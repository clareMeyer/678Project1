/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

/***************************************************************************
 * Included files
 ***************************************************************************/
#include "execute.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "quash.h"
#include "deque.h"

// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)


/***************************************************************************
 * Queue declarations (NEW!!!!)
 ***************************************************************************/
// Processes PIDs queue
IMPLEMENT_DEQUE_STRUCT(PIDDeque, pid_t);
IMPLEMENT_DEQUE(PIDDeque, pid_t);

// Job structure
typedef struct Job {
    int job_id;
    char* commandline;
    PIDDeque pid_list;
} Job;

// Job queue
IMPLEMENT_DEQUE_STRUCT(JobDeque, Job);
IMPLEMENT_DEQUE(JobDeque, Job);

/***************************************************************************
 * Job structure (NEW!!!!)
 ***************************************************************************/
// Constructor
static Job _newJob(){
    return (Job){
        0,
        get_command_string(),
        new_PIDDeque(1),
    };
}

// Destructor
static void _destroyJob(Job job){
    if(job.commandline != NULL)
    {
        free(job.commandline);
    }
    destroy_PIDDeque(&job.pid_list);
}


JobDeque jobs;

/***************************************************************************
 * Environment structure (NEW!!!!)
 ***************************************************************************/
// Environment that holds the pipes and a process
typedef struct Environment {
    int pipes[2][2];
    Job job;
} Environment;

// Constructor (initializer)
static void _newEnvironment(Environment* e){
    e->job = _newJob();
}

// Destructor
static void _destroyEnvironment(Environment* env){
    _destroyJob(env->job);
}


/***************************************************************************
 * Interface Functions
 ***************************************************************************/
// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();
  // Change this to true if necessary
  *should_free = true;

  char* cmdbuf = malloc(64);
  bzero(cmdbuf, 64);
/*  sprintf(cmdbuf, getcwd());*/
  getcwd(cmdbuf, 64);
  return cmdbuf;


  //return "get_current_directory()";
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  // TODO: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple
  // TODO: Remove warning silencers
  //(void) env_var; // Silence unused variable warning

  return getenv(env_var);
}

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  //IMPLEMENT_ME();

  if(is_empty_JobDeque(&jobs)){
    return;
  };
  size_t job_que_length = length_JobDeque(&jobs);
  for(size_t i=0; i<job_que_length; i++){

    Job current_job = pop_front_JobDeque(&jobs);
    size_t p_que_length = length_PIDDeque(&current_job.pid_list);

    for(size_t j=0; j<p_que_length; j++){
      //grab the first process from list of processes
      pid_t current_p = pop_front_PIDDeque(&current_job.pid_list);
      int status;
      pid_t after_p = waitpid(current_p, &status, WNOHANG);
      if(after_p==0){
        //need to push back because last process in the job
        push_back_PIDDeque(&current_job.pid_list, current_p);
      }
      else if(after_p==-1){
        //still have more process in the job
      }
      else if(after_p==current_p){
        //this is when all jobs are implemented
        print_job_bg_complete(current_job.job_id, current_p, current_job.commandline);
      }
    }
  }

  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  // // TODO: Remove warning silencers
  // (void) exec; // Silence unused variable warning
  // (void) args; // Silence unused variable warning
  //
  // // TODO: Implement run generic
  // IMPLEMENT_ME();
  execvp(exec, args);

  perror("ERROR: Failed to execute program");
}

// Print strings
void run_echo(EchoCommand cmd) {
  // FINISHED!!! (think so)
  // Print an array of strings. The args array is a NULL terminated (last
  // string is always NULL) list of strings.
  char** str = cmd.args;

  // TODO: Remove warning silencers
  // (void) str; // Silence unused variable warning

  // TODO: Implement echo
  // IMPLEMENT_ME();
  int i=0;
  while(str[i]!=NULL){
      printf("%s",str[i]);
      i++;
  }
  printf("\n");

  // Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // FINISHED!!! (think so)
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // TODO: Remove warning silencers
  // (void) env_var; // Silence unused variable warning
  // (void) val;     // Silence unused variable warning

  // TODO: Implement export.

  // HINT: This should be quite simple.
  // IMPLEMENT_ME();
  ////////////////////////////////// DOUBLE CHECK setenv(3), why last parameter is 1
  setenv(env_var,val,1);
}

// Changes the current working directory
void run_cd(CDCommand cmd) {
  // Get the directory name
  const char* dir = cmd.dir;

  // Check if the directory is valid
  if (dir == NULL) {
    perror("ERROR: Failed to resolve path");
    return;
  }

  // TODO: Change directory
  chdir(dir);

  // TODO: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.
  // IMPLEMENT_ME();
  setenv("OLD_PWD",lookup_env("PWD"),1);
  setenv("PWD",dir,1);
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // TODO: Remove warning silencers
  //(void) signal; // Silence unused variable warning
  //(void) job_id; // Silence unused variable warning

  //run loop that kills jobs
    //run loop that kills processes
  size_t job_que_length = length_JobDeque(&jobs);
  for(int i=0; i<job_que_length; i++){
    //pop the first job
    Job current_job = pop_front_JobDeque(&jobs);

    if(job_id == current_job.job_id){
      size_t p_que_length = length_PIDDeque(&current_job.pid_list);
      //loop through the processes in the current job
      for(int j=0; j<p_que_length; j++){
        pid_t current_p = pop_front_PIDDeque(&current_job.pid_list);
        //check this because not sure what inputs it requires for sure
        //kill takes a proccess and a signal, signal declared above
        kill(current_p, signal);
        //think we have to push back that its killed?
        push_back_PIDDeque(&current_job.pid_list, current_p);
      }
      //push job back to que
      push_back_JobDeque(&jobs, current_job);
    }
    else{
      //just put the job back on the que because its not the job you're looking for
      push_back_JobDeque(&jobs, current_job);
    }
  }


  // TODO: Kill all processes associated with a background job
  IMPLEMENT_ME();
}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  // IMPLEMENT_ME();
  bool should_free = false;
  char * cwd = get_current_directory(&should_free);
  fprintf(stdout, "%s\n", cwd );
  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // FINISHED!!! (think so)
  // TODO: Print background jobs
  // IMPLEMENT_ME();
  size_t jobs_length = length_JobDeque(&jobs);
  for(size_t i=0; i<jobs_length; i++){
     // Pop front job to get its values
    Job current_job = pop_front_JobDeque(&jobs);
    // Prints the job id number, the process id of the first process belonging to the Job, and the command string associated with this job
    print_job(current_job.job_id, peek_front_PIDDeque(&current_job.pid_list), current_job.commandline);
    // Push the job to the back
    push_back_JobDeque(&jobs,current_job);
  }
  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, int i, Environment* envr) {
    // Read the flags field from the parser
    bool p_in  = holder.flags & PIPE_IN;
    bool p_out = holder.flags & PIPE_OUT;
    bool r_in  = holder.flags & REDIRECT_IN;
    bool r_out = holder.flags & REDIRECT_OUT;
    bool r_app = holder.flags & REDIRECT_APPEND; // This can only be true if r_out
                                               // is true

    // TODO: Remove warning silencers
    (void) p_in;  // Silence unused variable warning
    (void) p_out; // Silence unused variable warning
    (void) r_in;  // Silence unused variable warning
    (void) r_out; // Silence unused variable warning
    (void) r_app; // Silence unused variable warning

    // TODO: Setup pipes, redirects, and new process
    // IMPLEMENT_ME();
    // STARTED IMPLEMENTATION. JUST BASIC STRUCTURE. NOT WORKING (YET)

    // create pipe if its p_out
    if(p_out)
    {
        pipe(envr.pipe[i-1)%2]);
    }

    pid_t pid_child;
    pid_child = fork();
    push_back_PIDDeque(&envr->job.pid_list,pid_child);

    if (pid_child == 0) {
        if(p_in){
            // Read from pipe
            dup2(envr->pipes[(i-1)%2][0],STDIN_FILENO);
            close(envr->pipes[(i-1)%2][1]);
        }

        if(p_out){
            // Write to pipe
            dup2(envr->pipes[(i-1)%2][1],STDOUT_FILENO);
            close(envr->pipes[(i-1)%2][0]);
        }

        if(r_in){
            // Read from standard in
            int fileIn = open(holder.redirect_in,O_RDONLY,00700);
            dup2(fileIn,STDIN_FILENO);
            close(fileIn);
        }

        if(r_out){
            if(r_app){
                // Write to the standard out appending its output
                int fileOut = open(holder.redirect_out,O_WRONLY|O_APPEND,0664); // 6 read write, 7 readwrite execute
            }
            else{
                // Write to standard out truncating the original file
                int fileOut = open(holder.redirect_out,O_WRONLY|O_TRUNC,0664);
            }
            dup2(fileOut,STDOUT_FILENO);
            close(fileOut);
        }
        child_run_command(holder.cmd);
        exit(0);
    }
    parent_run_command(holder.cmd);

  //parent_run_command(holder.cmd); // This should be done in the parent branch of
                                  // a fork
  //child_run_command(holder.cmd); // This should be done in the child branch of a fork
}

// Run a list of commands
void run_script(CommandHolder* holders) {
  if (holders == NULL)
    return;

  // Create Job queue the first time run_script is run
  static bool firstRun = true;
  if(firstRun)
  {
      jobs = new_destructable_JobDeque(1,_destroyJob);
      firstRun = false;
  }

  // Declare environment of the command. THe environment creates the pipes and a job. The job creates the pid_list que
  Environment envr;
  _newEnvironment(&envr);


  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  CommandType type;


  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i)
    create_process(holders[i],i,&envr);

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
    // QUESTION: We have to destroy the process because it is completed right? (so pop them)   WRONG!!!!!!!!!!!!!!!!!!!!!!!!!
    while (!is_empty_PIDDeque(&envr.job.pid_list)) {
        pid_t current_process = pop_front_PIDDeque(&envr.job.pid_list);
        int status=0;
        waitpid(current_process, &status, WNOHANG);
    }
  }
  else {
    // A background job.
    // TODO: Push the new job to the job queue
    IMPLEMENT_ME();

    // TODO: Once jobs are implemented, uncomment and fill the following line
    // print_job_bg_start(job_id, pid, cmd);
  }
}
