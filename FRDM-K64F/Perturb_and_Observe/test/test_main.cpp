/*************************** test_main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 *
 * Perturb and Observe Algorithm: Testing File
 *
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 16, 2015 at 14:23
 * Revised on: October 4, 2015 at 16:47
 *
 * Purpose: The purpose of this file is to test the P&O algorithm and replicate
 * hardware inputs through the use of the terminal command line (or) the Eclipse IDE.
 * This program reads inputs from an input file called, "inputs.txt." When finished,
 * it outputs the readings into an output file called, "outputs.txt."
 *
 * Instructions: These instructions are written for Linux/Unix.
 *			     Installing compiler for Linux and Mac:
 *					- For Mac, installing XCode command line tools (https://developer.apple.com/xcode/)
 *					- For Linux, run $sudo apt-get install g++
 *				 To compile code: $g++ -o runTest test_main.cpp
 * 				 To run code: $./runTest
 *
 * 				 These instructions are written for Windows users.
 * 				 Install an IDE (preferable Eclipse)
 *				 Create a new project, and make a new file called main.cpp
 *					- copy and paste the entire code below
 * 				 Right click on the project in the Project explorer view, and create a new file called inputs.txt
 * 				 Follow the instructions below, titled 'File Format for "inputs.txt"'
 *
 *
 * File Format for "inputs.txt": In order for this program to appropriately read the input text file,
 * 								 it must be formatted in the following way: (spaces and enters important)
 * 								 [Input Voltage] [Input Current]
 *								 [Original Duty Cycle] [Original Voltage] [Original Current]
 *								 (empty line)
 *							     [Input Voltage #1] [Input Current #1]
 *							     ...
 *							     [Input Voltage #n] [Input Current #n]
 *
 *****************************************************************************************/


/**
* The most widely used algorithm is the P&O algorithm. The P&O algorithm perturbs the duty cycle
* which controls the power converter, in this way it takes steps over the p-v characteristic to 
* find the MPPT. This perturbation causes a new operating point with a different output power. In 
* case this output power is larger than the previous output power, this point is set as the new 
* operating point. In case it is lower, the same power point is adjusted to a lower or higher working 
* voltage, depending on the previous step direction. (http://bit.ly/1L73nzE)
*
*
* Pulse Width (PW) is the elapsed time between the rising and falling edges of a single pulse.
* 
* Pulse Repetition Interval (PRI) is the time between sequential pulses.
*
* Pulse Repetition Frequency (PRF) is the reciprocal of PRI. The basic unit of measure for PRF
* is hertz (Hz). 
*
* Duty Cycle describes the "On Time" for a pulsed signal. We can report duty cycle in units of time, 
* but usually as a percentage. To calculate a signals' duty cycle, we need to know the signal's pulse
* width and repetition frequency.
* Duty Cycle = Pulse Width(sec) * Repetition Frequency (Hz) * 100
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/*
* Debugging P&O Algorithm
*
* Inputs: input voltage and input current
* Output: PWM
*/

//Define Constants
#define PWM_PERIOD_us        25
#define V_IN_MULT            50.989761
#define V_OUT_MULT           51.011235
#define I_IN_DIV             0.09776
#define I_OUT_DIV            0.09605
#define HALL_IN_NO_CURRENT   2.524
#define HALL_OUT_NO_CURRENT  2.514


//TODO: Read input values from a text file with random values to disable manual entry
int main ( int argc, char **argv){

	FILE *ptr_file; // pointer to the input file
	FILE *output_ptr_file; // pointer to the output file
	char inputFileName[11] = "inputs.txt"; // name of input file (with null termination)
	char outputFileName[12] = "outputs.txt"; // name of output file (with null termination)

	ptr_file = fopen(inputFileName,"r"); //The fopen statement opens a file “output.txt” in the write (w) mode.
	output_ptr_file = fopen(outputFileName, "a");

	/**
	 * The following four lines are to get the current date and time in [mm] [dd] [hh:mm:ss] [yyyy] format
	 **/
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	if (!ptr_file){
		printf("The input filename %s does not exist.", inputFileName);
		return 0;
	}

	if (!output_ptr_file){
    	printf("The output filename %s does not exist.", outputFileName);
    	return 0;
	} else{
		fprintf(output_ptr_file, "Duty Cycle readings taken at: %s\n", asctime (timeinfo) );
		fprintf(output_ptr_file, "Reading \t Duty Cycle\n");
	}

	int reading = 1; // counter to keep track of readings
	char buf[100]; // can read 100 characters per line
	printf("\n\t\t\t----------Program starting----------\n\n");

	// TODO: Angus spoke about a scale factor to multiple the readings by. Need to implement this.
	// read inputVoltage and inputCurrent from textfile
	double inVoltage, inCurrent, inPower;
	double outVoltage = 120;
	fgets(buf, 100, ptr_file);
	inVoltage = atof(strtok(buf, " -\n"));
	inCurrent = atof(strtok(NULL, " -\n"));
	inPower = inVoltage * inCurrent; // Power = Voltage * Current

	// read starting dutyCycle, originalVoltage, and originalCurrent from textfile
	double dutyCycle, originalVoltage, originalCurrent, originalPower, pulseWidth;
	fgets(buf, 100, ptr_file);
	dutyCycle = atof(strtok(buf, " -\n"));
	originalVoltage = atof(strtok(NULL, " -\n"));
	originalCurrent = atof(strtok(NULL, " -\n"));
	originalPower = originalCurrent * originalVoltage;

	printf("Previous voltage is: %.6f, Previous current is: %.6f, Previous power is: %.6f \n\n", originalVoltage, originalCurrent, originalPower);

	while (fgets(buf,1000, ptr_file) != NULL){
		printf("\t\t\t----------Start of reading #%d----------\n\n", reading); // make space in terminal
		if(strlen(buf) != 1){
			inVoltage = atof(strtok(buf, " -\n"));
			inCurrent = atof(strtok(NULL, " -\n"));
			inPower = inVoltage * inCurrent;
		}
		printf("Input voltage is: %.6f, Input current is: %.6f, Input power is: %.6f \n", inVoltage, inCurrent, inPower);
		double deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
		double deltaPower = inPower - originalPower;
		printf("Delta Voltage is: %.2f, Delta Power is: %.2f \n", deltaVoltage, deltaPower);

		if(deltaPower == 0){
			// continue code and skip everything else
		} else if(deltaPower > 0){
			if(deltaVoltage > 0){
				inVoltage += deltaVoltage; // increase PWM
			} else {
				inVoltage -= deltaVoltage; // decrease PWM
			}
		} else{ // deltaPower < 0
			if(deltaVoltage > 0){
				inVoltage -= deltaVoltage; // decrease PWM
			} else {
				inVoltage += deltaVoltage; // increase PWM
			}
		}
		originalVoltage = inVoltage; // replace old voltage with current voltage
		originalCurrent = inCurrent;
		originalPower = inPower; // replace old power with current power
		printf("Original voltage: %lf, Original current: %lf, Original Power: %lf\n", originalVoltage, originalCurrent, originalPower);
		dutyCycle = (outVoltage - inVoltage) / (outVoltage);
        pulseWidth = dutyCycle * PWM_PERIOD_us; // duty cycle = pulsewidth/period -> pulsewidth = duty cycle * period
		// TODO: Power Mosfet with new dutyCycle
		printf("Duty Cycle set to: %lf Pulse Width set to: %lf\n\n", dutyCycle, pulseWidth);

		fprintf(output_ptr_file, "Reading #%d \t %lf \n", reading, dutyCycle);

		printf("\t\t\t----------End of reading #%d----------\n\n", reading++);
	}

	fclose(ptr_file);
	fprintf(output_ptr_file, "\n");
	fclose(output_ptr_file);

}

