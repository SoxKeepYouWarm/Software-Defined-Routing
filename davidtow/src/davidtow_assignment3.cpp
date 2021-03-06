/**
 * @davidtow_assignment3
 * @author  David Towson <davidtow@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */

#include <iostream>
#include <stdio.h>

#include "router.h"
#include "Util.h"
/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	std::cout << "Running main" << std::endl;

	std::cout << "argc is " << argc << std::endl;

	Logger* logger = Logger::get_logger();

	char* control_port_input;
	if (argc == 2) {
		control_port_input = argv[1];
	} else {
		std::cout << "incorrect number of args" << std::endl;
		exit(1);
	}
	
	Router router(control_port_input);
	router.main();

	return 0;
}
