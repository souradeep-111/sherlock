//
// Created by Mac User on 1/16/18.
//

#ifndef NEURAL_RULE_ANALYSIS_TILING_H
#define NEURAL_RULE_ANALYSIS_TILING_H
#include <cassert>
#include <vector>
#include <map>
#include <deque>
#include "Box.h"

namespace NeuralRuleAnalysis {
    class Tiling;

    class Tile {
    protected:
        /* _parent_tiling: a reference to the parent tiling that this tile belongs to. */
        // Tiling const & _parent_tiling;
        /* _low_coords : left bottom coordinates of this tile */
        std::vector<int> _low_coords;
        /* _hi_coords: right hand top coordinates of this tile */
        std::vector<int> _hi_coords;

        bool _ok_coordinates_(int j) const;

    public:
        explicit Tile (Tiling const & ignored);

        Tile(const Tile & what) = default;

        int get_num_vars() const { return (int) _low_coords.size(); };

        std::pair<int, int>  get_coordinates(int j) const;
        int get_lower(int j) const;
        int get_upper(int j) const;

        void set_coordinates(int j, int lo, int hi);
        void set_lower(int j, int lo);
        void set_upper(int j, int hi);

        unsigned long long int get_volume_clipped(std::vector<int> const & low, std::vector<int> const &  hi) const;

        bool intersects(Tile const & what) const;
    };

    std::ostream & operator<< (std::ostream & out, Tile const & what);

    class Tiling {
    protected:
        /* _sys_vars: list of system variables that correspond to each dimension of the tiling */
        std::vector<int> _sys_vars;
        /* _lower_bounds: lower bounds for each dimension */
        std::vector<double> _lower_bounds;
        /* _upper_bounds: upper bounds for each dimension */
        std::vector<double> _upper_bounds;
        /* _num_subdivs: number of sub divisions for each dimension */
        std::vector<int> _num_subdivs;
        /* _tiles a list of tiles */
        std::vector<Tile> _tiles;

        bool __ok_tiling() const;

        std::pair<bool, Tile> search_for_empty_tile_rec(std::vector<int>  low, std::vector<int>  hi,
                                                        std::vector<Tile> current_tiles, std::deque<int> & dimensions) const;

        std::pair<bool, Tile> get_center_tile(std::vector<int>  low, std::vector<int>  hi) const;

        std::vector<Tile> collect_relevant_tiles(std::vector<int> const & low,
                                                 std::vector<int> const &  hi,
                                                 std::vector<Tile> const & current_tiles) const;

        bool current_tiles_subsume_region(std::vector<int> const & low, std::vector<int> const &  hi,
                                                  std::vector<Tile> const & current_tiles) const;
    public:

        Tiling(std::vector<int> const & vars,
               std::vector<double> const & lower,
               std::vector<double> const & upper,
               std::vector<int> const & num_subdivs);

        Tiling(const Tiling & what) = default;

        int get_num_vars() const { return (int) _sys_vars.size();};

        void insert_tile(Tile const & t);

        std::pair<bool, Tile> find_empty_tile() const;

        MpfiWrapper get_bound_for_var(int id) const;

        bool tile_has_no_intersections(Tile const & with_what) const;

        Box tile_to_box(Tile const & what ) const;

        int get_num_subdivs_for_dim(int j) const {
            assert(j >= 0 && j < _sys_vars.size());
            return _num_subdivs[j];
        }

    };

};

#endif //NEURAL_RULE_ANALYSIS_TILING_H
