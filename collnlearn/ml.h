#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "svm.h"
#include <iostream>
#include <math.h>
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
using namespace std;
class ML2{

    //Stores all the parameters of svm
    struct svm_parameter param;		// set by parse_command_line
    //stores the information of the data
    struct svm_problem prob;		// set by read_problem
    // the svm model object
    struct svm_model *model;
    // the x data (input)
    struct svm_node *x_space;
    // the label y data (output)
    struct svm_node *x;
    //flag to indicate whether to opt for cross validation
    int cross_validation;
    // the number of folds for cross validation 
    int nr_fold;
    // the best value of C selected for svm
    double best_C;
    // best value of gamma selected for svm
    double best_gamma;
    //variable to read the files into
    char *line;
    // maximum length of a line in a file
    int max_line_len;
    // the column of the file which stores the flow statistics
    int flowcol;
    //the name of the model file for the flow statistics
    string flowstatistics_train_name;
    //number of input space parameters
    int datacols;

public:
    ML2(int flowcol1, string flowstatistics_train_name1);

    //destructor
    ~ML2();

    
    //read the contents of the file, line by line
    char* readline(FILE *input);

    //train the svm using the parameters defined inside this method
    void trainData();
    // does cross validation using the parameters obtained or default ones
    double do_cross_validation();

    // read in a problem (in svmlight format)
    void read_problem_data(const char *filename,  int labelcol);


    //does grid search and finds the optimal set of parameters with the best performance in that grid
    void paramSelection();
    //predict the expected value using the trained svm and the input
    double predictML( double velocity[]);

};
