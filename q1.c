#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define TABLE_NUM 50
#define MAX_NAME 50


typedef struct Product {   // Product is a Meal in the restaurant  - each Meal is a node 

	char* ProductName;
	double Price;
	int Quantity;    // Quantity available in kitchen inventory   
	struct Product* next;

}Product, * pProduct;

typedef struct Inventory {  // A manager struct for list of Products, which represent the Inventory available 

	pProduct head;                 
	pProduct tail;

}Inventory, * PInventory;

typedef struct Table {             // A manager struct for list of Products arrived at each Table, every meal is a node.

	int TableNumber;
	int Delete;              // for DeleteItem use 
	int flag;
	pProduct head;
	pProduct tail;

}Table, * PTable;


void CreateProducts(FILE* Menu, PInventory Inventory); //Calling functions
void AddItems(char* ProductName, int Quantity, PInventory Inventory);
void OrderItem(int TableNumber, int Quantity, char* ProductName, Table* Table_arr, PInventory Inventory);
void RemoveItem(int TableNumber, Table* Table_arr);
void RemoveTable(int TableNumber, Table* Table_arr);
void Error_msg(char* msg);

void CreateProducts(FILE* Menu, PInventory Inventory) //This function create Nodes with info about meals.
{
	char temp_name[MAX_NAME];
	pProduct temp_product;

	while (fscanf(Menu, "%s", temp_name) != EOF) { //Scan until end of file
		temp_product = (pProduct)malloc(sizeof(Product)); // Allocation of product
		if (temp_product == NULL) {															
			while (Inventory->head != NULL) { //If failed										
				temp_product = Inventory->head;
				Inventory->head = Inventory->head->next;
				free(temp_product->ProductName);
				free(temp_product);
			}
			Error_msg("\nMemory allocation failed");
		}

		temp_product->ProductName = (char*)malloc(strlen(temp_name) + 1); //Allocation for name
		if (temp_product->ProductName == NULL) {								  		    													
			while (Inventory->head != NULL) {												
				temp_product = Inventory->head;
				Inventory->head = Inventory->head->next;
				free(temp_product->ProductName);
				free(temp_product);
			}
			Error_msg("\nMemory allocation failed");
		}

		strcpy(temp_product->ProductName, temp_name); // Enterting the name to node
		fscanf(Menu, "%d%lf", &(temp_product->Quantity), &(temp_product->Price)); //More info entered
		temp_product->next = NULL; 

		if (Inventory->head == NULL) //First item is head												  
			Inventory->head = temp_product;
		else																
			Inventory->tail->next = temp_product;

		Inventory->tail = temp_product;	// inventory tail will now point to new last item
	}

	printf("The kitchen was created\n\n");
	return;
}

void AddItems(char* ProductName, int Quantity, PInventory Inventory) // This function adds to a product's quantity in inventory
{
	pProduct temp = Inventory->head; // Set temp to be first item on the list //

	if (Quantity <= 0) {
		printf("Error - cannot add un positive quantity !\n\n");
		return;
	}

	while (temp != NULL) { // Check all items 
		if (strcmp(temp->ProductName, ProductName) == 0) { // if the current Node on the list has the same name given as an argument //
			temp->Quantity += Quantity;												  
			printf("%d %s were added to the kitchen\n\n", Quantity, ProductName);		  
			return;	//return void
		}
		temp = temp->next; //Next node on list
	}
	printf("We don't have %s in our resturant, sorry!\n\n", ProductName); 

	return;
}

void OrderItem(int TableNumber, int Quantity, char* ProductName, Table* Table_arr, PInventory Inventory) //This function basiclly order's the meal.
{
	int i, index = TableNumber - 1;
	pProduct NewOrder, temp = Inventory->head, Deletetemp;

	if (TableNumber > 50 || TableNumber < 1) {
		printf("Error - Invalid table number \n\n");
		return;
	}

	while (temp != NULL) {  // Check all items 
		if (strcmp(temp->ProductName, ProductName) == 0) { //Checking if required item is on the list
			if (temp->Quantity >= Quantity) { // if Quantity is available go on
				NewOrder = (pProduct)malloc(sizeof(Product)); //Memory Allocation for a new product node for desired table

				if (NewOrder == NULL) { //Allocation failed
					while (Inventory->head != NULL) { // Free all the memory allocations 
						Deletetemp = Inventory->head; // Free inventory
						Inventory->head = Inventory->head->next;
						free(Deletetemp->ProductName);
						free(Deletetemp);
					}

					for (i = 0; i < TABLE_NUM; i++) { //Free all items ordered for each table 
						while (Table_arr[i].head != NULL) {
							Deletetemp = Table_arr[i].head;
							Table_arr[i].head = (Table_arr[i].head)->next;
							free(Deletetemp->ProductName);
							free(Deletetemp);
						}
					}


					Error_msg("Memory allocation failed");
				}

				NewOrder->ProductName = (char*)malloc(strlen(ProductName) + 1);

				if (NewOrder->ProductName == NULL)				
					Error_msg("Memory allocation failed");

				strcpy(NewOrder->ProductName, ProductName); //Defining our new item
				NewOrder->Price = temp->Price;
				NewOrder->Quantity = Quantity;
				NewOrder->next = NULL;

				if (Table_arr[index].head == NULL) // if its the 1st item set tail to be it              
					Table_arr[index].tail = NewOrder;

				NewOrder->next = Table_arr[index].head; // inserting to head of list
				Table_arr[index].head = NewOrder; 

				temp->Quantity -= Quantity;// Updating the item's inventory quantity
				printf("%d %s were added to table number %d\n\n", Quantity, NewOrder->ProductName, TableNumber);
				return;
			}

			printf("There are only %d %s available\n\n", temp->Quantity, temp->ProductName); // In case the item exist but there's not enough of the item.
			return;
		}
		temp = temp->next; //Keep going in the list
	}
	printf("We don't have %s, sorry!\n\n", ProductName);
	return;
}

void RemoveItem(int TableNumber, Table* Table_arr) //This function remove a meal from the table.
{
	int index = TableNumber - 1;
	pProduct ToRemove;

	if (TableNumber > 50 || TableNumber < 1) { // Table number error
		printf("Error - Invalid table number \n\n");
		return;
	}
	else if (Table_arr[index].head == NULL) {  // Empty list
		printf("Table number %d hasnt order yet \n\n", TableNumber);
		return;
	}
	else if (Table_arr[index].Delete == 1) { //In case table already deleted.
		printf("This table already cancelled an order \n\n");
		return;
	}

	ToRemove = Table_arr[index].head;  //Setting Node we want to remove
	Table_arr[index].head = (Table_arr[index].head)->next; // Setting the 2nd item to be the head now

	printf("%d %s was removed from table number %d\n\n", ToRemove->Quantity, ToRemove->ProductName, TableNumber);    

	free(ToRemove->ProductName); // Removing allocations of the node
	free(ToRemove);

	Table_arr[index].Delete = 1; // Setting the delete to be 1, which means this table already deleted an item

	return;
}

void RemoveTable(int TableNumber, Table* Table_arr) //This function basiclly close a table and check it's cost. 
{
	int index = TableNumber - 1;
	pProduct ToDel, temp = Table_arr[index].head;
	double sum = 0;

	if (temp == NULL) {	//Table hasn't ordered anything yet.
		printf("The table number %d is not ordered yet\n\n", TableNumber);
		return;
	}

	while (temp != NULL) { // Check all items 
		printf("%d %s, ", temp->Quantity, temp->ProductName);  
		ToDel = temp; // Set this item to be deleted
		sum += (temp->Price) * (temp->Quantity); // Calculate check price
		temp = temp->next; // Keep going

		free(ToDel->ProductName); // Free the node
		free(ToDel);
	}

	printf("%g nis+%g nis for tips,please!", sum, 0.15 * sum); //Price

	Table_arr[index].tail = Table_arr[index].head = NULL;  // Reset table
	Table_arr[index].Delete = 0;

	return;
}

void Error_msg(char* msg)
{
	printf("%s", msg);
	exit(1);

	return;
}

int main(){
	int i, option, quantity, table_num; //Defining
	char name[MAX_NAME];
	Table Table_arr[TABLE_NUM], Temptable;
	FILE* Instructions, * Manot;
	Inventory main_Inventory;
	pProduct temp;
	Temptable.head = Temptable.tail = NULL;
	Temptable.Delete = 0;
	for (i = 0; i < TABLE_NUM; i++) {
		Temptable.TableNumber = i + 1;
		Table_arr[i] = Temptable;  //Save the adresses of the values
	}
	if ((Instructions = fopen("Instructions.txt", "rt")) == NULL) //Checking the files
		Error_msg("failed to open manot.txt file.");
	if ((Manot = fopen("Manot.txt", "rt")) == NULL)
		Error_msg("failed to open manot.txt file.");

	main_Inventory.head = main_Inventory.tail = NULL;         

	CreateProducts(Manot, &main_Inventory); //Calling function
	fclose(Manot);

	while (fscanf(Instructions, "%d", &option) != EOF) {

		switch (option) {

		case 1:
			break;

		case 2:
			fscanf(Instructions, "%s%d", name, &quantity);
			AddItems(name, quantity, &main_Inventory);
			break;

		case 3:
			fscanf(Instructions, "%d%s%d", &table_num, name, &quantity);
			OrderItem(table_num, quantity, name, Table_arr, &main_Inventory);
			break;

		case 4:
			fscanf(Instructions, "%d", &table_num);
			RemoveItem(table_num, &Table_arr);
			break;

		case 5:
			fscanf(Instructions, "%d", &table_num);
			RemoveTable(table_num, &Table_arr);
			break;
		}
	}

	fclose(Instructions);

	while (main_Inventory.head != NULL) { // free inventory list 
		temp = main_Inventory.head;
		main_Inventory.head = (main_Inventory.head)->next;
		free(temp->ProductName);
		free(temp);
	}

	for (i = 0; i < TABLE_NUM; i++) { // free all items ordered on each table 
		while (Table_arr[i].head != NULL) {
			temp = Table_arr[i].head;
			Table_arr[i].head = (Table_arr[i].head)->next;
			free(temp->ProductName);
			free(temp);
		}
	}

	return 0;
}
