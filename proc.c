/*Name: maxwell sanders 
 *Programming assignment 1
 *Description: This is the program that reads from the file system that is built
 *into linux systems known as the proc file system and gives computer statistics back
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//this is our main which will take in other commands
int main(int argc, char** argv){
    //create the file pointer and the line buffer to read files
    FILE *fp;
    char buffer[200];
    char *token;
    unsigned int count = 0;
    int totalmem;
    double temp;
    int i;

    //create a sleep time for parse and avgtime
    //and a variable for datapoints
    int parsetime, avgtime;
    int points = 0;

    //open the memory file, read it, save it, since
    //there is a chance that this could be dynamic
    //meaning we should really only take this once
    fp = fopen("/proc/meminfo", "r");
    fgets(buffer, 200, fp);
    strtok(buffer, " ");
    totalmem = atol(strtok(NULL, " "));
    fclose(fp);

    //we need to get the number of cpus on the system
    //starting cpus on 0 gives a -1 to the number of
    //skip the first line since it is cputotal row first
    fp = fopen("/proc/stat", "r");
    int cpus = 0;
    fgets(buffer, 200, fp);
    fgets(buffer, 200, fp);
    while( strcmp(strtok(buffer, " "),"intr") != 0){
	fgets(buffer, 200, fp);
	cpus ++;
    }
    fclose(fp);
 
    //check to see if this is the verbose form
    if(argc == 3){
	//grab the parsetime and avgtime
	parsetime = atoi(argv[1]);
	avgtime = atoi(argv[2]);

	//check to see if the number combinations are invalid
	if(avgtime <= 0 || parsetime <= 0 || avgtime <= parsetime){
	    printf("Aborted: Invalid set of parameters\n");
	    return 0;
	}
  
	//variables for the cpus
	double peruser = 0;
	double persys = 0;
	double peridle = 0;

	//miscellaneous variables
	double context = 0;
	double process = 0;
	double disk = 0;

	//variables for the mem
	int freemem = 0;
	double percentmem = 0;

	//keep this process going
	while(1){
	    //if the count is a multiple of the average then average, print, and reset
	    if(count != 0 && count%avgtime == 0){
		//print everything
		printf("CPU Time spent in user mode: %.3f%% Total, %.3f%% per CPU\n",
							peruser/points, (peruser/points)/cpus);
		printf("CPU Time spent in system mode: %.3f%% Total, %.3f%% per CPU\n",
							persys/points, (persys/points)/cpus);
		printf("CPU Time spent idle: %.3f%% Total, %.3f%% per CPU\n",
							peridle/points, (peridle/points)/cpus);
		printf("Free Memory: %d kB, %.3f%% of total\n", freemem/points, percentmem/points);
		printf("Disk read/write sectors per second: %.3f\n", disk/points);
		printf("Context switches per second: %.3f\n", context/points);	
		printf("Process creations per second: %.3f\n", process/points);	

		//reset all the values and then print a new line
		disk = 0;
		context = 0;
		process = 0;
		freemem = 0;
		percentmem = 0;
		peruser = 0;
		persys = 0;
		peridle = 0;
		points = 0;
		printf("\n");
	    }
	    //if the count is a multiple of the parse time then parse
	    if(count%parsetime == 0){
		//open the uptime file, and read it, temp will hold the uptime
		fp = fopen("/proc/uptime", "r");
		fgets(buffer, 200, fp);
		temp = atof(strtok(buffer, " "));
		//peridle += atof(strtok(NULL, " "))/temp;
		fclose(fp);
		
    		//open the cpu file, read it, update cpu variables
		fp = fopen("/proc/stat", "r");
		fgets(buffer, 200, fp);
		strtok(buffer, " ");
		//these variables will come in as 1/100 of seconds so 
		//I will have to divide by 100 once to get seconds
		//and then multiply by 100 to get percent which will cancel  
		//The first one also has to parse 2 since the first two slots
		//contain user mode and then niced user mode
		peruser += ((atof(strtok(NULL," "))+atof(strtok(NULL," ")))/temp);
		persys += (atof(strtok(NULL," "))/temp);
		peridle += (atof(strtok(NULL," "))/temp);
		//keep skipping content until you get to the context switch line
		//the interrupt line is a hefty son-of-a-gun
		while(1){
		    fgets(buffer, 200, fp);
		    if(strcmp(strtok(buffer, " "), "ctxt") == 0){ 
			break;
		    }
		}
		//now we store the context switches
		context += atof(strtok(NULL, " \n"))/temp;
		//skip a line and then get the processes
		fgets(buffer, 200, fp);
		fgets(buffer, 200, fp);
		strtok(buffer, " ");
		process += atof(strtok(NULL, " \n"))/temp;
		fclose(fp);

		//open the memory file, read it, update mem variables, and close it
    		fp = fopen("/proc/meminfo", "r");
    		fgets(buffer, 200, fp);
		fgets(buffer, 200, fp);
		strtok(buffer, " ");
    		temp = atof(strtok(NULL, " "));
		freemem += temp;
		percentmem += (temp/totalmem)*100;
    		fclose(fp);
	 
		//opent the disk file, read it, update the disk variable, and close it
		fp = fopen("/proc/diskstats", "r");
		//keep going till you get to the last line
		while(fgets(buffer, 200, fp) != NULL){
		    //have to sift through all the garbage
		    strtok(buffer, " ");
		    for(i = 0; i < 4; i ++){
			strtok(NULL, " ");
		    }
		    disk += atof(strtok(NULL, " "))/temp;
		    for(i = 0; i < 3; i ++){
			strtok(NULL, " ");
		    }
		    disk += atof(strtok(NULL, " "))/temp;
		}
		fclose(fp);		

		//increment how many data points there are	
		points++;
	    }
	    count++;
	    sleep(1);
	}
    }

    //check to see if this isn't improper form
    else if(argc != 1){
	printf("Form not valid.\n");
        printf("Type nothing to get basic statistics.\n");
        printf("Type 2 number mods to indicate secs to parse and secs to average data\n");
	return 0;
    }

    //if this is the default
    printf("Basic system statistics\n");
    printf("=======================\n");

    //open the cpuinfo file, read it, print it, and close it
    printf("Processors:\n");
    fp = fopen("/proc/cpuinfo", "r");
    while( fgets(buffer, 200, fp) != NULL){
	//check what the first word is and see if it is model
	//if so we can take the next line and display that since
	//that line will be the model name
	strtok(buffer, "\t");
	if(strcmp(buffer, "model") == 0){
	    fgets(buffer, 200, fp);
	    printf("%d - %s", count, buffer);
	    count ++;
	}
    }
    fclose(fp);

    //open the version file, read it, print it, and close it
    fp = fopen("/proc/version", "r");
    fgets(buffer, 200, fp);
    printf("Kernel-Version: %s", buffer);
    fclose(fp);

    //display the total memory of the system
    printf("Total Memory: %d kB\n", totalmem);

    //open the uptime file, and read it
    fp = fopen("/proc/uptime", "r");
    fscanf(fp, "%s %s\n", buffer, NULL);
    fclose(fp);

    //what's updog? section off the time into nice sections using math
    int uptime = atoi(buffer);
    int updays = uptime/86400;
    uptime %= 86400;
    int uphours = uptime/3600;
    uptime %= 3600;
    int upminutes = uptime/60;
    uptime %= 60;

    //prints off all the data we know
    printf("System Uptime: %d-days %d-hours %d-minutes %d-seconds (%.2f seconds total)\n", 
			   updays, uphours, upminutes, uptime, atof(buffer));
}

