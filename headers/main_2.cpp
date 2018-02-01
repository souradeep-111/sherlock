#include<iostream>
#include<fstream>
#include<vector>
#include "./headers/propagate_intervals.h"

using namespace std;
datatype offset = 100;

int main()
{

	unsigned int no_of_steps,i , j , k;
	unsigned int no_of_state_var = 4;
	unsigned int no_of_control_var = 1;

	no_of_steps = 100;


  char name_0[] = "neural_network_information_x_1";
	network_handler network_x_0(name_0);
	char name_1[] = "neural_network_information_x_2";
	network_handler network_x_1(name_1);
	char name_2[] = "neural_network_information_x_3";
	network_handler network_x_2(name_2);
	char name_3[] = "neural_network_information_x_4";
	network_handler network_x_3(name_3);
	char name_5[] = "neural_network_information_u";
	network_handler network_u(name_5);


	vector< vector< datatype > > initial_state_box(no_of_state_var, vector< datatype >(2) );
	vector< vector< datatype > > state_box(no_of_state_var, vector< datatype >(2));
	vector< vector< datatype > > next_state_box(no_of_state_var, vector< datatype >(2));

	vector< vector< datatype > > super_box( no_of_state_var + no_of_control_var, vector< datatype >(2));


	initial_state_box[0][0] = 0; initial_state_box[0][1] = 1;
	initial_state_box[1][0] = 0.5; initial_state_box[1][1] = 1.3;
	initial_state_box[2][0] = 0.2; initial_state_box[2][1] = 0.4;
	initial_state_box[3][0] = -1; initial_state_box[3][1] = -0.5;

	vector< datatype > u_box(no_of_control_var,0);

	state_box = initial_state_box;
	i = 0;
	while(i < no_of_steps)
	{
		cout << "At i = " << i << endl;
		cout << "state_box = " << state_box[0][0] << "  " << state_box[0][1] << endl << endl;

		network_u.return_interval_output(state_box, u_box);
		u_box[0] -= offset; u_box[1] -= offset;

		super_box.clear();
		j = 0;
		while(j < no_of_state_var)
		{
			super_box.push_back(state_box[j]);
			j++;
		}
		j = 0;
		while(j < no_of_control_var)
		{
			super_box.push_back(u_box);
			j++;
		}

		network_x_0.return_interval_output(super_box, next_state_box[0]);
		next_state_box[0][0] -= offset; next_state_box[0][1] -= offset;
		// cout << "Done with x = 0 " << endl;

		network_x_1.return_interval_output(super_box, next_state_box[1]);
		next_state_box[1][0] -= offset; next_state_box[1][1] -= offset;
		// cout << "Done with x = 1 " << endl;

		network_x_2.return_interval_output(super_box, next_state_box[2]);
		next_state_box[2][0] -= offset; next_state_box[2][1] -= offset;
		// cout << "Done with x = 2 " << endl;

		network_x_3.return_interval_output(super_box, next_state_box[3]);
		next_state_box[3][0] -= offset; next_state_box[3][1] -= offset;
		// cout << "Done with x = 3 " << endl;


		state_box = next_state_box;

		i++;
	}



  // Next call a function by passing it an interval and getting the output ranges



  return 0;
}
