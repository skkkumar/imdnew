
#include "ml.h"

    ML2::ML2(int flowcol1, string flowstatistics_train_name1){
        //ML2();
         flowcol = flowcol1;
         flowstatistics_train_name = flowstatistics_train_name1;
         datacols = 6;
         model=NULL;
    }

    //destructor
    ML2::~ML2(){
         if(model!=NULL){
             //free pointers and space
	         svm_free_and_destroy_model(&model);
         }
            svm_destroy_param(&param);
	    free(prob.y);
	    free(prob.x);
	    free(x_space);
	    free(line);
    }

    
    //read the contents of the file, line by line
    char* ML2::readline(FILE *input)
    {
	    int len;
	
	    if(fgets(line,max_line_len,input) == NULL)
		    return NULL;

	    while(strrchr(line,'\n') == NULL)
	    {
		    max_line_len *= 2;
		    line = (char *) realloc(line,max_line_len);
		    len = (int) strlen(line);
		    if(fgets(line+len,max_line_len-len,input) == NULL)
			    break;
	    }
	    return line;
    }

    //train the svm using the parameters defined inside this method
    void ML2::trainData()
    {

        //file to store the svm model structure
	    string model_file_name1 = flowstatistics_train_name+"_model.csv";
        const char *model_file_name = model_file_name1.c_str();
        //file to read the data from
	    char input_file_name[1024] = "velocityArray.csv";
        //char input_file_name2[1024] = "data/flowstatistics_train_mu.csv";
	    const char *error_msg;

       //parameters of the svm
        /*
       "-s svm_type : set type of SVM (default 0)\n"
	    "	0 -- C-SVC		(multi-class classification)\n"
	    "	1 -- nu-SVC		(multi-class classification)\n"
	    "	2 -- one-class SVM\n"
	    "	3 -- epsilon-SVR	(regression)\n"
	    "	4 -- nu-SVR		(regression)\n"
	    "-t kernel_type : set type of kernel function (default 2)\n"
	    "	0 -- linear: u'*v\n"
	    "	1 -- polynomial: (gamma*u'*v + coef0)^degree\n"
	    "	2 -- radial basis function: exp(-gamma*|u-v|^2)\n"
	    "	3 -- sigmoid: tanh(gamma*u'*v + coef0)\n"
	    "	4 -- precomputed kernel (kernel values in training_set_file)\n"
	    "-d degree : set degree in kernel function (default 3)\n"
	    "-g gamma : set gamma in kernel function (default 1/num_features)\n"
	    "-r coef0 : set coef0 in kernel function (default 0)\n"
	    "-c cost : set the parameter C of C-SVC, epsilon-SVR, and nu-SVR (default 1)\n"
	    "-n nu : set the parameter nu of nu-SVC, one-class SVM, and nu-SVR (default 0.5)\n"
	    "-p epsilon : set the epsilon in loss function of epsilon-SVR (default 0.1)\n"
	    "-m cachesize : set cache memory size in MB (default 100)\n"
	    "-e epsilon : set tolerance of termination criterion (default 0.001)\n"
	    "-h shrinking : whether to use the shrinking heuristics, 0 or 1 (default 1)\n"
	    "-b probability_estimates : whether to train a SVC or SVR model for probability estimates, 0 or 1 (default 0)\n"
	    "-wi weight : set the parameter C of class i to weight*C, for C-SVC (default 1)\n"
	    "-v n: n-fold cross validation mode\n"
	    "-q : quiet mode (no outputs)\n"
        */
	    //set the parameters to be used for svm
        param.svm_type = 4;
	    param.kernel_type = 1;//RBF;
	    param.degree = 2;
	    param.gamma = 0.125;	// 1/num_features
	    param.coef0 = 0;
	    param.nu = 0.4;
	    param.cache_size = 100;
	    param.C = 0.125;
	    param.eps = 1e-3;
	    param.p = 0.1;
	    param.shrinking = 1;
	    param.probability = 0;
	    param.nr_weight = 0;
	    param.weight_label = NULL;
	    param.weight = NULL;
        //param.v = 10;
        nr_fold =10;

        //read from the data file
	    read_problem_data( input_file_name, flowcol);

        //checking the parameters, if they are set correctly
	    error_msg = svm_check_parameter(&prob,&param);
	    if(error_msg)
	    {
		    cout<<"ERROR: "<<error_msg<<endl<<flush;
		    exit(1);
	    }

        //do_cross_validation();
        // first do grid search do find optimal parameters 
        //paramSelection();  

        
        // then do training with optimal parameters
        //param.gamma = best_gamma;
	    //param.C = best_C;

        cout<< "start training\n"<<endl<<flush; 
        model = svm_train(&prob,&param);
        cout<< "end training\n"<<endl<<flush; 
        
        // then do cross fold validation
        cout<< "start with cross validation" <<endl<<flush; 
	    do_cross_validation();
        cout<< "end cross validation" <<endl<<flush; 
       
        //save model
	    if(svm_save_model(model_file_name,model))
	    {
		    cout<< "can't save model to file "<< model_file_name <<endl<<flush; 
		    exit(1);
	    } 
	
	
        //free all the pointers used, except the model which is required for prediction
//             svm_destroy_param(&param);
// 	    free(prob.y);
// 	    free(prob.x);
// 	    free(x_space);
// 	    free(line);
	    return;
    }

    // does cross validation using the parameters obtained or default ones
    double ML2::do_cross_validation()
    {
	    int i;
	    int total_correct = 0;
	    double total_error = 0;
	    double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	    double *target = Malloc(double,prob.l);
        //this uses the nr_folds set in the train method
	    svm_cross_validation(&prob,&param,nr_fold,target);
        
	    if(param.svm_type == EPSILON_SVR ||
	       param.svm_type == NU_SVR)
	    {
		    for(i=0;i<prob.l;i++)
		    {
			    double y = prob.y[i];
			    double v = target[i];
                //printf("actual y : %f, predicted value %f \n", y, v );
			    total_error += (v-y)*(v-y);
			    sumv += v;
			    sumy += y;
			    sumvv += v*v;
			    sumyy += y*y;
			    sumvy += v*y;
		    }
            //print the error rates
		    cout<<"Cross Validation Mean squared error = "<<total_error/prob.l<<endl<<flush;
            cout<<"Cross Validation Root Mean squared error = "<<sqrt(total_error/prob.l) <<endl<<flush;
		    cout<<"Cross Validation Squared correlation coefficient = "<<
			    ((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
			    ((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
			     <<endl<<flush;
	    }
	    
	    free(target);

        return sqrt(total_error/prob.l);
    }


    // read in a problem (in svmlight format)
    void ML2::read_problem_data(const char *filename,  int labelcol)
    {

        cout<< "file name is "<< filename<<endl<<flush;

	    int elements, max_index, inst_max_index, i, j;
	    FILE *fp = fopen(filename,"r");

	    char *endptr;
	    char *idx, *val, *label;
        char separator[] = ",";

	    if(fp == NULL)
	    {
		    cout<< "can't open input file "<<endl<<flush;
		    exit(1);
	    }
       
        //count the number of lines
	    prob.l = 0;
	    max_line_len = 1024;
	    line = Malloc(char,max_line_len);
	    while(readline(fp)!=NULL)
	    {
             char *p = strtok(line,"\n"); 
		     ++prob.l;
	    }
        cout<<"number of lines "<< prob.l<<" , number of elements "<< prob.l*(datacols+1)<<" , DataCols : "<< datacols<<endl<<flush;
        //point the file pointer again to the beginning of the file
	    rewind(fp);
        cout<<"allocate space"<<endl<<flush;
        //allocate space for the problem
        prob.y = Malloc(double,prob.l);
        prob.x = Malloc(struct svm_node *,prob.l);
	    x_space = Malloc(struct svm_node,prob.l*(datacols+1)); // there are datacol columns, +1 is for storing the -1 element (end)
  
	    max_index = 0;
	    j=0;
        cout<<"start reading data"<<endl<<flush;
	    for(i=0;i<prob.l;i++)
	    {
		    inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		    readline(fp);
          
            //store address of x_space into prob.x
		    prob.x[i] = &x_space[j];
            //read one line
		    label = strtok(line,"\n"); 
//             printf("label is %s \n " , label);
		    if(label == NULL){ // if empty line
                cout<<"exit"<<endl<<flush;
			    break;
             }
             //the first datacols is the input space so store into prob.x
             int k = 0;
             for ( ; k < datacols; k++){
                x_space[j].index = k+1;
               //if this is the first input then use label
                if(k == 0){
		             x_space[j].value = atof(strtok(label,",")); // prob.y[i] = strtod(label,&endptr);
//                     cout<<"x_space["<<j<<"].value is " << x_space[j].value<<endl<<flush;
                }else{
                //if this is not the first input then use label
                    x_space[j].value = atof(strtok(NULL,",")); // prob.y[i] = strtod(label,&endptr);
//                     cout<<"x_space["<<j<<"].value is " << x_space[j].value<<endl<<flush;
                }
//                 cout<<"-------------1----------------"<<endl<<flush;
                j++;
            }
//               cout<<"-------------2----------------"<<endl<<flush;
            //save the flowcol column as y label
            k = 0;
            double a=0;
            //for the column 2, ignore the 2 columns then store column 2 (0-based indexing)
// 	    cout<<labelcol<<endl<<flush;
            while(k < labelcol){
// 	      cout<<k<<endl<<flush;
                 a=atof(strtok(NULL,",")); 
// 		 cout<<a<<endl<<flush;
                 k++;
            }
//             cout<<"-------------4----------------"<<endl<<flush;
            prob.y[i] = atof(strtok(NULL,",")); // prob.y[i] = strtod(label,&endptr);
//             cout<<"prob y["<<i<<"] is  " << prob.y[i]<<endl<<flush; 
            
             //to indicate end of the row in prob.x
             x_space[j].index = -1;
             x_space[j].value = 0;
             j++;
//              cout<<"j : "<<j<<flush;
	    }
        
        cout<<"finished storing the problem"<<endl<<flush;

	    if(param.gamma == 0 && max_index > 0)
		    param.gamma = 1.0/max_index;

	    if(param.kernel_type == PRECOMPUTED)
		    for(i=0;i<prob.l;i++)
		    {
			    if (prob.x[i][0].index != 0)
			    {
				    cout<<"Wrong input format: first column must be 0:sample_serial_number\n"<<endl<<flush;
				    exit(1);
			    }
			    if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
			    {
				     cout<<"Wrong input format: sample_serial_number out of range\n"<<endl<<flush;
				    exit(1);
			    }
		    }

	    fclose(fp);

    }



    //does grid search and finds the optimal set of parameters with the best performance in that grid
    void ML2::paramSelection()  
    {
         //parameters defining grid of parameters, min, max, step
         double c_min = -5;
         double c_max = 15;
         double c_step = 2;
         double gamma_min = -15;
         double gamma_max = 3;
         double gamma_step = 2;

         int c_length = (int)((c_max-c_min) / c_step); 
         int gamma_length = (int)((gamma_max-gamma_min) / gamma_step); 
         double C[ c_length ];
         double gamma[gamma_length];
         //fill the array of parameters
         for(int  i = 0 ;  i < c_length ; i++){
              C[i] =  c_min + c_step * i;
         }
         for(int  i = 0 ;  i < gamma_length ; i++){
              gamma[i] =  gamma_min + gamma_step * i;
         }
         //[C,gamma] = meshgrid(-5:2:15, -15:2:3);
                
         // grid search, and cross-validation  
         double cv_acc[c_length * gamma_length]; //= zeros(numel(C),1);
         for (int i = 0 ; i < c_length; i++){
             for(int j = 0 ; j < gamma_length; j++){
                int index = i * c_length + j;
                param.C = pow(2, C[i]);
                param.gamma = pow(2 , gamma[j]);
                double accuracy = do_cross_validation(); 
                cv_acc[index] = accuracy;
                cout<<"********************************************************************"<<endl<<flush;
             }
         }
               
         // find pair (C,gamma) with best accuracy, lowest rmse
         double min_error = 1000;
         int cv_length = c_length * gamma_length; 
          cout<<"cv_length "<< cv_length<< c_length<<endl<<flush;
         int index = 0;
         for(int i = 0 ; i < cv_length ; i ++){
              if(cv_acc[i] != 0 && cv_acc[i] < min_error){
                    cout<<"found min\n"<<endl<<flush;
                   min_error = cv_acc[i];
                   index = i;
              }

         }
         //print the performance of the whole grid
         for (int i = 0 ; i < c_length; i++){
             for(int j = 0 ; j < gamma_length; j++){
                int index = i * c_length + j;
                cout<<cv_acc[index]<<" \t" <<flush;
             }
             cout<<endl<<flush;
         }

         cout<<"best accuracy , min rmse = "<< min_error<<endl<<flush;
               
          // now you can train you model using best_C and best_gamma
          int c_index = int(index / c_length) ;
          int gamma_index = index - c_index * c_length;
          best_C =  pow( 2, C[c_index]);
          best_gamma = pow(2, gamma[gamma_index]);
                
          cout<<"*************************************************\n"<<endl<<flush;
          cout<<"best_C = "<< best_C<<endl<<flush;
          cout<<"best_gamma = "<< best_gamma<<endl<<flush;
    }

    //predict the expected value using the trained svm and the input
    double ML2::predictML( double velocity[])
    {
	    double error = 0;
	    
        //get info about svm using the model
	    int svm_type=svm_get_svm_type(model);
	    int nr_class=svm_get_nr_class(model);
	    double *prob_estimates=NULL;
	    int j;
        int predict_probability = param.probability;

	cout<<"svm type : "<<svm_type<<endl<<flush;
	
	cout<<"nr class : "<<nr_class<<endl<<flush;
	
	cout<<"predict probability : "<<predict_probability<<endl<<flush;
	    if(predict_probability)
	    {
		    if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
			    cout<<"Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma="<<svm_get_svr_probability(model)<<endl<<flush;
	
	    }
        
	    int i = 0;
	    double target_label, predict_label;
	    //allocate space for x
        int max_nr_attr = datacols + 1;
         x = Malloc(struct svm_node,max_nr_attr);
        //store each of the velocity parameter
        for( i = 0 ; i < datacols; i++){
           x[i].index = i+1;
	       x[i].value = velocity[i];
        }
        
        //end of the x
	    x[i].index = -1;
        //predict the value
	    predict_label = svm_predict(model,x);
        //free x
        free(x);
	    cout<<"prediction "<<predict_label <<endl<<flush;

	    if(predict_probability)
		    free(prob_estimates);

        return predict_label;
    }

