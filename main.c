#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CITY_NAME_LENGTH 100
#define MAX_CITIES 100
bool flightFound = false;

typedef struct _Flight {
    char origin[MAX_CITY_NAME_LENGTH];
    char destination[MAX_CITY_NAME_LENGTH];
} Flight;

int isFlightAvailable(Flight flights[], int numFlights, char origin[], char destination[]) {
    int i;
    for (i = 0; i < numFlights; i++) {
        if (strcmp(flights[i].origin, origin) == 0 && strcmp(flights[i].destination, destination) == 0) {
            return 1;
        }
    }
    return 0;
}

GtkWidget *textViewWindow = NULL;
GtkTextView *textView = NULL;

void displayPath(const char *path) {
    if (!textViewWindow) {
        // Create a new window
        textViewWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(textViewWindow), "Flight Path");
        gtk_window_set_default_size(GTK_WINDOW(textViewWindow), 600, 400);

        // Create a text view and add it to the window
        textView = GTK_TEXT_VIEW(gtk_text_view_new());
        gtk_container_add(GTK_CONTAINER(textViewWindow), GTK_WIDGET(textView));

        // Show all widgets
        gtk_widget_show_all(textViewWindow);
    }

    // Set the text in the text view
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(textView);
    gtk_text_buffer_set_text(buffer, path, -1);
}


void findFlights(Flight flights[], int numFlights, char currentCity[], char destination[], char path[], int pathLength, FILE *outputFile) {
    if (strcmp(currentCity, destination) == 0) {
        flightFound = true;
        g_print("%s\n", path);
        fprintf(outputFile, "%s\n", path);
        displayPath(path);
        return;
    }

    int i;
    for (i = 0; i < numFlights; i++) {
        if (strcmp(flights[i].origin, currentCity) == 0) {
            char newPath[MAX_CITIES * MAX_CITY_NAME_LENGTH];
            strcpy(newPath, path);
            strcat(newPath, " -> ");
            strcat(newPath, flights[i].destination);
            findFlights(flights, numFlights, flights[i].destination, destination, newPath, pathLength + 1, outputFile);
        }
    }
    return;
}
void on_find_flights_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *originEntry = GTK_ENTRY(gtk_builder_get_object(GTK_BUILDER(data), "originEntry"));
    GtkEntry *destinationEntry = GTK_ENTRY(gtk_builder_get_object(GTK_BUILDER(data), "destinationEntry"));

    FILE *inputFile = fopen("input_airlines.txt", "r");
    if (!inputFile) {
        g_print("Error opening input file.\n");
        return;
    }

    int numFlights;
    fscanf(inputFile, "%d", &numFlights);
    Flight flights[numFlights];

    int i;
    for (i = 0; i < numFlights; i++) {
        fscanf(inputFile, "%s %s", flights[i].origin, flights[i].destination);
    }

    fclose(inputFile);

    char path[MAX_CITIES * MAX_CITY_NAME_LENGTH];
    strcpy(path, gtk_entry_get_text(originEntry));

    FILE *outputFile = fopen("connected_flights.txt", "w");
    if (!outputFile) {
        g_print("Error opening output file.\n");
        return;
    }

    fprintf(outputFile, "Flights between %s and %s:\n", gtk_entry_get_text(originEntry), gtk_entry_get_text(destinationEntry));
    findFlights(flights, numFlights, path, (char*)gtk_entry_get_text(destinationEntry), path, 1, outputFile);

    if (!isFlightAvailable(flights, numFlights, (char*)path, (char*)gtk_entry_get_text(destinationEntry))) {
        g_print("No direct flight available.\n");
    }
    
     if (!flightFound) {
        GtkWidget *noFlightWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(noFlightWindow), "No Flights Found");
        gtk_window_set_default_size(GTK_WINDOW(noFlightWindow), 200, 100);
        GtkWidget *label = gtk_label_new("No flights available.");
        gtk_container_add(GTK_CONTAINER(noFlightWindow), label);
        gtk_widget_show_all(noFlightWindow);
    }
    fclose(outputFile); 
}



int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new_from_file("airlines.glade");
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    GtkEntry *originEntry = GTK_ENTRY(gtk_builder_get_object(builder, "originEntry"));
    GtkEntry *destinationEntry = GTK_ENTRY(gtk_builder_get_object(builder, "destinationEntry"));
    GtkWidget *findFlightsButton = GTK_WIDGET(gtk_builder_get_object(builder, "findFlightsButton"));


    // Set the default size of the main window
    //gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
     // Set the size of the entry boxes
    gtk_widget_set_size_request(GTK_WIDGET(originEntry), 200, 50);  // Width of 200 pixels, default height
    gtk_widget_set_size_request(GTK_WIDGET(destinationEntry), 200, 50);  // Width of 200 pixels, default height

    // Set the size of the find button
    gtk_widget_set_size_request(findFlightsButton, 100, 20);  // Width of 100 pixels, default height

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "findFlightsButton")), "clicked", G_CALLBACK(on_find_flights_button_clicked), builder);

    gtk_widget_show_all(window);

    // Set the default size of the main window to 600x400 pixels
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);


    gtk_main();

    return 0;
}