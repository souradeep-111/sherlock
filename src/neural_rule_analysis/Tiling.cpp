//
// Created by Mac User on 1/16/18.
//
#include <ostream>
#include "Tiling.h"
#include <cassert>
#include <gmp.h>

namespace NeuralRuleAnalysis{

    Tile::Tile (Tiling const & what){
        int num_vars = what.get_num_vars();
        for(int i = 0; i < num_vars; ++i){
            _low_coords.push_back(0);
            _hi_coords.push_back(0);
        }
    };

    bool Tile::_ok_coordinates_(int j) const {
       return (j >= 0 && j < _low_coords.size() && _hi_coords.size() == _low_coords.size());
    };

    std::pair<int, int>  Tile::get_coordinates(int j) const {
        assert(_ok_coordinates_(j));
        return std::make_pair(_low_coords[j], _hi_coords[j]);
    };

    int Tile::get_lower(int j) const {
        assert(_ok_coordinates_(j));
        return _low_coords[j];
    }

    int Tile::get_upper(int j) const {
        assert(_ok_coordinates_(j));
        return _hi_coords[j];
    }

    void Tile::set_coordinates(int j, int lo, int hi){
        assert(_ok_coordinates_(j));
        _low_coords[j] = lo;
        _hi_coords[j] = hi;
    }
    void Tile::set_lower(int j, int lo){
        assert(_ok_coordinates_(j));
        _low_coords[j] = lo;
    }
    void Tile::set_upper(int j, int hi){
        assert(_ok_coordinates_(j));
        _hi_coords[j] = hi;
    }

    bool Tile::intersects(Tile const & what) const{
        int n = get_num_vars();
        assert(what.get_num_vars() == n);
        for (int j = 0; j < n; ++j) {
            if (what.get_lower(j) >= get_upper(j) || what.get_upper(j) <= get_lower(j)){
                return false;
            }
        }
        return true;
    }

    std::ostream & operator << (std::ostream & out, Tile const & what){
        int n = what.get_num_vars();
        for (int i =0; i < n; ++i){
            out << "\t" << i << ":" << what.get_lower(i) << "," << what.get_upper(i) << std::endl;
        }
        return out;
    }

    bool Tiling::__ok_tiling() const {
        if ( _sys_vars.size() == _lower_bounds.size() &&
             _sys_vars.size() == _upper_bounds.size() &&
             _sys_vars.size() == _num_subdivs.size() ){
            int num_vars = (int) _sys_vars.size();
            for (int i = 0; i < num_vars; ++i){
                if (_lower_bounds[i] > _upper_bounds[i] )
                    return false;
            }
            return true;
        }
        return false;
    }

    Tiling::Tiling(std::vector<int> const & vars,
                   std::vector<double> const & lower,
                   std::vector<double> const & upper,
                   std::vector<int> const & num_subdivs):_sys_vars(vars),
                                                         _lower_bounds(lower),
                                                         _upper_bounds(upper),
                                                         _num_subdivs(num_subdivs){
        assert(__ok_tiling());
    }

    void Tiling::insert_tile(Tile const &t) {
        _tiles.push_back(t);
    }


    std::pair<bool, Tile> Tiling::get_center_tile(std::vector<int>  low, std::vector<int>  hi) const {
        int n = (int) low.size();
        Tile ret_tile(*this);
        for (int i = 0; i < n; ++i){
            assert(low[i] < hi[i]);
            int mid_pt = (low[i] + hi[i])/2;
            assert(mid_pt >= low[i] && mid_pt < hi[i]);
            ret_tile.set_coordinates(i, mid_pt, mid_pt+1);
        }
        return std::make_pair(true, ret_tile);
    }

    std::vector<Tile> Tiling::collect_relevant_tiles(std::vector<int> const & low, std::vector<int> const &  hi,
                                                     std::vector<Tile> const & current_tiles) const {
        int num_vars = (int) low.size();
        std::vector<Tile> ret_list;
        int tile_low, tile_hi;
        for (auto & t : current_tiles){
            bool intersects = true;
            for(int j = 0; j < num_vars; ++j){
                std::tie(tile_low, tile_hi) = t.get_coordinates(j);
                if (tile_low >= hi[j] || tile_hi <= low[j] ){
                    intersects = false;
                    break;
                }
            }
            if (intersects)
                ret_list.push_back(t);
        }
        return ret_list;
    }

    unsigned long long int Tile::get_volume_clipped(std::vector<int> const & low, std::vector<int> const &  hi) const{
        int n = get_num_vars();
        unsigned long long int tile_vol = 1;
        for (int i = 0; i < n; ++i){
            int lhs_pt = std::max(low[i], get_lower(i));
            int rhs_pt = std::min(hi[i], get_upper(i));
            assert(lhs_pt < rhs_pt);
            tile_vol *= (unsigned long long int) (rhs_pt - lhs_pt);
        }
        return tile_vol;
    }

    bool Tiling::current_tiles_subsume_region(std::vector<int> const & low, std::vector<int> const &  hi,
                                              std::vector<Tile> const & current_tiles) const {
        // 1. Calculate total volume of the current tiles restricted to low/hi
        // TODO: implement this in exact arithmetic
        unsigned long long int tile_vol = 0;
        for (Tile const & t: current_tiles){
            tile_vol += t.get_volume_clipped(low, hi);
        }

        // 2. Calculate total volume of the search region.
        unsigned long long int total_vol = 1;
        auto num_vars = (int) low.size();
        for (int i = 0; i < num_vars; ++i){
            total_vol *= (unsigned long long int) (hi[i] - low[i]);
        }

        return (tile_vol >= total_vol);
    }

    std::pair<bool, Tile> Tiling::search_for_empty_tile_rec(std::vector<int> low, std::vector<int>  hi,
                                                            std::vector<Tile> current_tiles,
                                                            std::deque<int> & dimensions) const {
        // 1. If there are no current tiles, then the entire region is empty.
        // Return the center tile.
        if (current_tiles.size() == 0)
            return get_center_tile(low, hi); // return the center tile here.

        // Search if there are any dimensions to split along.
        if (dimensions.size() == 0)
            return std::make_pair(false, Tile(*this)); // If none left, just bail
        // Add up the volumes of tiles and check if it exceeds that of the current region
        if (current_tiles_subsume_region(low, hi, current_tiles)) {
            return std::make_pair(false, Tile(*this)); // If yes, nothing is left.
        }
        // Otherwise, find a dimension to split along.
        int split_dim = -1;
        while (dimensions.size() > 0){
            int j = dimensions.front();
            dimensions.pop_front();
            if (hi[j] - low[j] > 1){
                split_dim = j; // The first splittable dimension
                break;
            }
        }
        // split_dim should not be -1
        assert(split_dim >= 0);
        if (split_dim >= 0) {
            dimensions.push_back(split_dim); // split_dim is now at the very end of the new list.
            int mid_pt = (hi[split_dim] + low[split_dim]) / 2;
            assert(low[split_dim] < mid_pt && mid_pt < hi[split_dim]);
            int tmp_lo = low[split_dim], tmp_hi = hi[split_dim];
            if (split_dim % 2 == 0) {
                // search right half
                low[split_dim] = mid_pt;
                auto new_tiles = collect_relevant_tiles(low, hi, current_tiles);
                auto p = search_for_empty_tile_rec(low, hi, new_tiles, dimensions);
                if (p.first) {
                    return p;
                }
                // search left half
                low[split_dim] = tmp_lo;
                hi[split_dim] = mid_pt;
                auto new_tiles2 = collect_relevant_tiles(low, hi, current_tiles);
                auto p2 = search_for_empty_tile_rec(low, hi, new_tiles2, dimensions);
                if (p2.first) {
                    return p2;
                }
            } else {
                hi[split_dim] = mid_pt;
                auto new_tiles = collect_relevant_tiles(low, hi, current_tiles);
                auto p = search_for_empty_tile_rec(low, hi, new_tiles, dimensions);
                if (p.first) {
                    return p;
                }
                hi[split_dim] = tmp_hi;
                low[split_dim] = mid_pt;
                auto new_tiles2 = collect_relevant_tiles(low, hi, current_tiles);
                auto p2 = search_for_empty_tile_rec(low, hi, new_tiles2, dimensions);
                if (p2.first) {
                    return p2;
                }
            }
        }

        return std::make_pair(false, Tile(*this));
    }

    std::pair<bool, Tile> Tiling::find_empty_tile() const {
        std::deque<int> dims;
        std::vector<int> lb;
        std::vector<int> ub(_num_subdivs);
        for (int i = 0; i < this -> get_num_vars(); ++i){
            dims.push_back(i);
            lb.push_back(0);
        }
        return search_for_empty_tile_rec(lb, ub, _tiles, dims);
    }

    bool Tiling::tile_has_no_intersections(Tile const & with_what) const{
        for(Tile const & t: _tiles){
            if (t.intersects(with_what))
                return false; // intersection
        }
        return true; // No intersection
    }

    Box Tiling::tile_to_box(Tile const & what ) const{
        // Translate the bounds of the tile to coordinates for the box.
        int n = (int) _sys_vars.size();
        Box b(_sys_vars);
        for (int i = 0; i < n; ++i){
            int lo_i, up_i;
            std::tie(lo_i, up_i) = what.get_coordinates(i);
            assert(lo_i < up_i);
            double lo_x, up_x;
            double delta_i = (_upper_bounds[i] - _lower_bounds[i])/(double) _num_subdivs[i];
            lo_x = _lower_bounds[i] +  delta_i * (double) lo_i ;
            up_x = _lower_bounds[i] + delta_i * (double) up_i;
            b.set_dimensions(_sys_vars[i], MpfiWrapper(lo_x, up_x));
        }
        return b;
    }

    MpfiWrapper Tiling::get_bound_for_var(int id) const {
        int n = (int) _sys_vars.size();
        for (int i=0; i < n; ++i){
            if (_sys_vars[i] == id){
                return MpfiWrapper(_lower_bounds[i], _upper_bounds[i]);
            }
        }
        // I cannot get here
        assert(false);
    }

};
