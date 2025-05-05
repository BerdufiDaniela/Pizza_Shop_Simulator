/*
--Γεράσιμος Μάγκος ΑΜ:p3210107
--Αικατερίνη Παλαιολόγου ΑΜ:p3210149
--Ντανιέλα Μπερντούφι ΑΜ:p3210132
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "pizzeria.h"

//Global variables
int revenue = 0;				     	 //Esoda
int plainPizzasSold = 0;   	   	 	     //Kanonikes pitses 
int specialPizzasSold = 0; 	  	         //Special pitses  
int	SuccessFulOrder = 0; 	 	  		 //Epityxhmenes paraggelies
int failedOrders = 0; 			         //Apotyxhmenes paraggelies
int maxCustomerServiceTime = 0;          //Megistos xronos eksyphrethshs pelath 
int maxCoolingTime = 0; 	             //Megistos xronos krywmatos 
int sumCoolingTime=0; 		             //Athroisma xronwn krywmatos 
int sumServiceTime=0; 		             //Athroisma xronwn eksyphrethshs 
int seed; 					             //Sporos

//Mutexes and condition variables
pthread_mutex_t revenue_mutex;  		//Mutex for revenues 
pthread_mutex_t pizza_sales_mutex;		//Mutex for pizza sales
pthread_mutex_t order_stats_mutex; 		//Mutex for statisticks
pthread_mutex_t screen_mutex;			//Mutex for printing in screen 
pthread_mutex_t preperation_mutex;		//Mutex for preperation 
pthread_mutex_t oven_mutex;				//Mutex for ovens
pthread_mutex_t pack_mutex;				//Mutex for packaging
pthread_mutex_t deliver_mutex;			//Mutex for delivery 
pthread_mutex_t cooltime_mutex;			//Mutex for cooltime 
pthread_mutex_t servicetime_mutex;		//Mutex for service time
pthread_cond_t  preperation_cond;		//Condition variable for preperation
pthread_cond_t  oven_cond;				//Condition variable for ovens 
pthread_cond_t  pack_cond;				//Condition variable for packaging 
pthread_cond_t  deliver_cond;			//Condition variable for delivery 


//Generate a random number between min and max using rand_r function 
int generate_random(int min, int max) {
    return (rand_r(&seed) % max)+1;
}


//Get the current time(Useful to get the time of process begins)
long long current_time() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

//Calculate the elapsed time(Useful to calculate the total time of process starts and ends)
int Elapsed_Time(long long start_time) {
    return (int)((current_time() - start_time) / 1000);
}


//Function to update the value(Useful to change in maxCoolingTime and maxCustomerServiceTime when needed)
void UpdateValue(int* old_value, int new_value) {
    if (new_value > *old_value) {
        *old_value = new_value;
    }
}

//Compute the Average Value(Useful for statistics calcultions)
double AverageValue (int value1, int value2) {
    if (value1>0) {
		int Avg=value1/value2;
        return (double)Avg; 
    }
	else{
		return 0;
	}
}
	

//Failed Orders
void Failed_Order(int NumberOfOrder) {
    pthread_mutex_lock(&screen_mutex);
    printf("H paraggelia me arithmo %d apetyxe.\n", NumberOfOrder);
    pthread_mutex_unlock(&screen_mutex);
	
	//Increase the number of failed orders 
    pthread_mutex_lock(&order_stats_mutex);
    failedOrders++;
    pthread_mutex_unlock(&order_stats_mutex);
}

//Registered orders
void OrderToDo(int NumberOfOrder) {
    pthread_mutex_lock(&screen_mutex);
    printf("H paraggelia %d eksipiretitai.\n", NumberOfOrder);
    pthread_mutex_unlock(&screen_mutex);
}

//Ready orders
void ReadyOrder(int NumberOfOrder, int PreperationTime) {
    pthread_mutex_lock(&screen_mutex);
    printf("H paraggelia me arithmo %d etoimasthke se %d lepta.\n", NumberOfOrder, PreperationTime);
    pthread_mutex_unlock(&screen_mutex);
}

//Delivery
void DeliveryOrder(int NumberOfOrder, int delivery_time) {
	
    pthread_mutex_lock(&screen_mutex);
    printf("H paraggelia me arithmo %d paradwthike se %d lepta.\n", NumberOfOrder, delivery_time);
    pthread_mutex_unlock(&screen_mutex);

	//Increase the number of successful orders 
    pthread_mutex_lock(&order_stats_mutex);
    SuccessFulOrder++;
    pthread_mutex_unlock(&order_stats_mutex);
	
	
    pthread_mutex_lock(&pizza_sales_mutex);
    pthread_mutex_lock(&revenue_mutex);
	
	//Increase the number of total pizzas  
    int total_pizzas = plainPizzasSold + specialPizzasSold;
	printf("Apoteloutan apo:\n");
    printf("%d Kanonikes pitses, %d Special pitses\n", plainPizzasSold, specialPizzasSold);
	pthread_mutex_unlock(&revenue_mutex);
    pthread_mutex_unlock(&pizza_sales_mutex);
}

//Cooling_Time
void OrderCoolTime(int NumberOfOrder,int coolingtime){
	
    UpdateValue(&maxCoolingTime, coolingtime); //Change the maxCoolingTime(if its needed)
	pthread_mutex_lock(&screen_mutex);
    printf("H paraggelia %d: eixe xrono krywmatos: %d lepta\n",NumberOfOrder, coolingtime);
	pthread_mutex_unlock(&screen_mutex);
}


//Treat Order Function
void* customer_thread(void* arg) {
    int NumberOfOrder = *(int*)arg; //Number Of order 
	
	//Order details 
    int placement_time = generate_random(Torderlow, Torderhigh);
    int NumberofPizzasInOrder = generate_random(Norderlow, Norderhigh);
	
	//Choose randomly the type of pizza 
	int plain=0;    //Number of plain pizzas 
	int special=0; //Number of special pizzas 
		
    	for(int i=0 ; i<NumberofPizzasInOrder; i++){
    		int is_plain = (generate_random(1, 100) <= Pplain) ? 1 : 0; //Choose type of pizza
    		if (is_plain) {
    			plain++;
    		} else {
    			special++;
    		}
		}
		
		
    //Print order details 
    pthread_mutex_lock(&screen_mutex);
    printf("O pelaths %d parhggeile: %d pitses\n", NumberOfOrder, NumberofPizzasInOrder);
    pthread_mutex_unlock(&screen_mutex);


    //Wait for payment 
    int TimeForPay=generate_random(Tpaymentlow, Tpaymenthigh); //Random value for TimeForPay 

    //Check for payment 
    if (TimeForPay/10.0<= Pfail/100.0) {
        Failed_Order(NumberOfOrder); //Payment failed --> Order fail 
        pthread_exit(NULL);
    }else{
		sleep(TimeForPay); //Payement processing 
		
	
		//Keep what time is now that the order is starting
		long long timeprep=current_time();
		
		//Register the order 
		OrderToDo(NumberOfOrder);	
	
		//Increase the quantity of plain,special pizzas and the value of revenue
		
		pthread_mutex_lock(&pizza_sales_mutex);
    	pthread_mutex_lock(&revenue_mutex);
		
    	plainPizzasSold += plain;
    	specialPizzasSold += special;
    	revenue += plain * Cplain;
    	revenue += special * Cspecial;
		
	
    	pthread_mutex_unlock(&revenue_mutex);
    	pthread_mutex_unlock(&pizza_sales_mutex);
		
		
		//Print order details 
    	pthread_mutex_lock(&screen_mutex);
    	printf("O pelaths %d parhggeile %d kanonikes pitses kai %d special pitses.\n", NumberOfOrder,plain,special);
    	pthread_mutex_unlock(&screen_mutex);
    	
    
    	//Prepare the order
    	pthread_mutex_lock(&preperation_mutex); //check if there is any Cook to prepare the order
    	while(Ncook==0){ 				 //While we dont have available Cook 
    		printf("H paraggelia %d den brhke diathesimo mageira.[!] ....Blocked....[!]\n",NumberOfOrder);
    		pthread_cond_wait(&preperation_cond,&preperation_mutex);
    	}
    	printf("H paraggelia %d brhke diathesimo mageira.\n ",NumberOfOrder);
	   //There is a Cook who can prepare the order reduce one Cook
    	Ncook--; //Decrease the number of available Cook
    	pthread_mutex_unlock(&preperation_mutex);
    	
 		//The Cook is preparing the pizzas
    	int PreperationTime = NumberofPizzasInOrder * Tprep ;
    	sleep(PreperationTime); //Cooking process
		
		

    	// Wait for available ovens
    	pthread_mutex_lock(&oven_mutex);
    	while (NumberofPizzasInOrder > Noven) { //We dont have enough available ovens
			printf("Den yparxoun diathesimoi fournoi gia thn paraggelia\n");
        	pthread_cond_wait(&oven_cond, &oven_mutex);
    	}
		
		//There found ovens now 
    	pthread_mutex_lock(&preperation_mutex);
    	printf("H paraggelia %d proetoimasthke me epityxia!\n",NumberOfOrder);
    	Ncook++;
    	pthread_cond_signal(&preperation_cond);
    	pthread_mutex_unlock(&preperation_mutex);
		

    	//Get the suitable number of ovens
		printf("Oi fournoi einai diathesimoi gia thn paraggelia\n");
    	Noven =Noven-NumberofPizzasInOrder;
    	pthread_mutex_unlock(&oven_mutex);

    	//Bake the pizzas
    	int BakeTime = NumberofPizzasInOrder * Tbake ;
    	sleep(BakeTime);
		
		long long timecool=current_time(); //Get the time of pizzas starting cooling 
		
		//Check if there is any Packer to pack the order
		pthread_mutex_lock(&pack_mutex);
		while(Npacker==0){
			printf("H paraggelia %d den brhke Packer.[!]....Blocked....[!]\n",NumberOfOrder);
			pthread_cond_wait(&pack_cond,&pack_mutex);
		}
		printf("H paraggelia %d brhke Packer.\n",NumberOfOrder);
		
		//There is a packer who can pack the order reduce the Packers now
		Npacker--;
		pthread_mutex_unlock(&pack_mutex);
		
		// Pack the pizzas
    	int pack_time = NumberofPizzasInOrder * Tpack ;
    	sleep(pack_time); //Packaging process 
		

    	//The Packer has finished and now is free
    	pthread_mutex_lock(&pack_mutex);
    	printf("H paraggelia %d paketaristhke epityxws!\n",NumberOfOrder);
    	Npacker++;
    	pthread_cond_signal(&pack_cond);
    	pthread_mutex_unlock(&pack_mutex);
    	
    	int time_prep=Elapsed_Time(timeprep);  //Time from the start to end of the proccess
		ReadyOrder(NumberOfOrder,time_prep);   //The order is ready to delivered
		

    	//Release the ovens
    	pthread_mutex_lock(&oven_mutex);
    	Noven =Noven+NumberofPizzasInOrder;
    	pthread_cond_broadcast(&oven_cond);
    	pthread_mutex_unlock(&oven_mutex);
		
		pthread_mutex_lock(&deliver_mutex);
    	while(Ndeliverer==0){
    		printf("H paraggelia %d den brhke delivery.[!]....Blocked....[!]\n",NumberOfOrder);
    		pthread_cond_wait(&deliver_cond,&deliver_mutex);
    	}
    	printf("H paraggelia %d brhke delivery.\n",NumberOfOrder);
    	Ndeliverer--;
    	pthread_mutex_unlock(&deliver_mutex);
		
		// Deliver the order
    	int delivery_time = generate_random(Tdellow, Tdelhigh);
    	sleep(delivery_time);                      		//Deliver process 
    	int time_delivered=Elapsed_Time(timeprep); 		//Calculate the time till now that the delivery guy delivered the order
		
		int time_cool=Elapsed_Time(timecool);      		//Time of order pizzas cooling 
    	pthread_mutex_lock(&cooltime_mutex);
    	sumCoolingTime=sumCoolingTime+time_cool;   		//Increase the total cooling time 
    	pthread_mutex_unlock(&cooltime_mutex);
		
		pthread_mutex_lock(&servicetime_mutex);
    	sumServiceTime=sumServiceTime+time_delivered; 	//Increase the total service time 
    	pthread_mutex_unlock(&servicetime_mutex);
    	
		
		DeliveryOrder(NumberOfOrder, time_delivered);   
    	OrderCoolTime(NumberOfOrder,time_cool);
    	sleep(delivery_time);							//Delivery came back 

		pthread_mutex_lock(&deliver_mutex);
    	printf("H paraggelia %d paradwthike epityxws!\n",NumberOfOrder);
    	Ndeliverer++;									//Delivery guy is available again
    	pthread_cond_signal(&deliver_cond);
    	pthread_mutex_unlock(&deliver_mutex);
		
		
    	//Update customer service time
    	pthread_mutex_lock(&order_stats_mutex);
    	UpdateValue(&maxCustomerServiceTime, placement_time + delivery_time);
    	pthread_mutex_unlock(&order_stats_mutex);
		free(arg);	
	}

}

//-------------------Main function----------------------------------------------
int main(int argc, char* argv[]) {
	
	
	/*	[!]Check for correctly arguments written in terminal[!]	 */
	
	//First argument --> Number Of customers 
    int NumberOfCustomers = atoi(argv[1]);
	
	//Second argument --> Number of seed 
    int seed =atoi(argv[2]);
	
	//If NumberOfCustomers<0 --> Execusion stops
    if (NumberOfCustomers <= 0) {
        printf("[!]CUSTOMERS CAN'T BE A NEGATIVE NUMBER[!]");
        return 1;
    }
	
	if (argc != 3) {
        printf("[!]ERROR OCCURED[!] --> INVALID NUMBER OF ARGUMENTS");
        return 1;
    }
	
	
	//Initialize threads
	pthread_mutex_init (&revenue_mutex,NULL );
    pthread_mutex_init (& pizza_sales_mutex,NULL);
    pthread_mutex_init (&order_stats_mutex,NULL);
    pthread_mutex_init (&screen_mutex,NULL);
    pthread_mutex_init (&preperation_mutex,NULL);
    pthread_mutex_init (&oven_mutex,NULL);
    pthread_mutex_init (& pack_mutex,NULL);
    pthread_mutex_init (&deliver_mutex,NULL);
	

	printf("\n\n|Pizza Ordering And Delivery System|\n\n");


    // Create customer threads
    pthread_t customers[NumberOfCustomers];
    for (int i = 0; i < NumberOfCustomers; i++) {
    	seed=seed+i;
        int* NumberOfOrder = malloc(sizeof(int));
        *NumberOfOrder = i + 1;
        //The first customer sing in when time is 0
        if(i==0){
        	if (pthread_create(&customers[i], NULL, customer_thread, NumberOfOrder) != 0) {
            	printf("Failed to create customer thread\n");
            	return 1;
        	}
        //The other customers sign in after the time is between [Torderlow,Torderhigh]
        }else{
        	sleep(generate_random(Torderlow, Torderhigh));
        	if (pthread_create(&customers[i], NULL, customer_thread, NumberOfOrder) != 0) {
            	printf("Failed to create customer thread\n");
            	return 1;
        	}
        }
    }

    //Wait for customer threads to finish
    for (int i = 0; i < NumberOfCustomers; i++) {
        pthread_join(customers[i], NULL);
    }
	
	

	//Functio to print total Statistics of 
void Statisticks() {
	
	//Lock mutex of statistics,revenue,pizza sales 
    pthread_mutex_lock(&order_stats_mutex);
    pthread_mutex_lock(&revenue_mutex);
    pthread_mutex_lock(&pizza_sales_mutex);
	
	//Print the results
    printf("\n----Statistics----\n");
    printf("Synolika esoda: %d eurw\n", revenue);
	printf("Poulithikan: \n");
    printf("-Kanonikes pitses: %d\n", plainPizzasSold);
    printf("-Special pitses: %d\n", specialPizzasSold);
    printf("Epityxhmenes paraggelies: %d\n", SuccessFulOrder);
    printf("Apotyxhmenes paraggelies: %d\n", failedOrders);
    printf("Mesos xronos eksyphrethshs pelath: %.2lf lepta\n",AverageValue (sumServiceTime,SuccessFulOrder));
    printf("Megistos xronos eksyphrethshs pelath: %d lepta\n", maxCustomerServiceTime);
    printf("Mesos xronos krywmatos: %.2lf minutes\n",AverageValue (maxCoolingTime,SuccessFulOrder));
    printf("Megistos xronos krywmatos: %d lepta\n", maxCoolingTime);
	
	//Unlock mutex of statistics,revenue,pizza sales 
    pthread_mutex_unlock(&pizza_sales_mutex);
    pthread_mutex_unlock(&revenue_mutex);
    pthread_mutex_unlock(&order_stats_mutex);
}
	
    //Print Statisticks before destroy mutexes and condition variables 
    Statisticks();
	
	
	//Destroy mutexes and condition variables
	pthread_mutex_destroy(&revenue_mutex);
	pthread_mutex_destroy(&pizza_sales_mutex); 
	pthread_mutex_destroy(&order_stats_mutex);
	pthread_mutex_destroy(&preperation_mutex);
	pthread_mutex_destroy(&oven_mutex);
	pthread_mutex_destroy(&pack_mutex);
	pthread_mutex_destroy(&screen_mutex);
	pthread_mutex_destroy(&deliver_mutex);
	pthread_cond_destroy(&preperation_cond);
	pthread_cond_destroy(&oven_cond);
	pthread_cond_destroy(&pack_cond);
	pthread_cond_destroy(&deliver_cond);
	pthread_exit(NULL);
	
    return 0;
}// End main


