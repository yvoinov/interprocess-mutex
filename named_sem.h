#include <unistd.h>		/* For getppid() */
#include <fcntl.h>		/* For O_* constants */
#include <sys/stat.h>		/* For mode constants */
#include <semaphore.h>
#include <cstdlib>		/* For std::exit */
#include <stdexcept>		/* For std::runtime_error */
#include <iostream>

#define MAXPROCESSES 1
#define MODE 0664

const std::string sem_name = "/mutex_" + std::to_string(getppid());

class NamedSemaphore {
public:
	NamedSemaphore(const std::string& s_name = sem_name) : sname(s_name) {
		sem = sem_open(sem_name.c_str(), O_CREAT, MODE, 1);
		if (sem == SEM_FAILED) {
			throw std::runtime_error("ERROR: Failed to create semaphore " + sem_name);
		} else {
			if (sem_getvalue(sem, &sem_value) == 0)
				if (sem_value > MAXPROCESSES) {
					sem_wait(sem);
					std::cerr << "ERROR: Process already running" << std::endl;
					std::exit(EXIT_FAILURE);
				} else sem_post(sem);
			else {
				throw std::runtime_error("ERROR: Failed to get semaphore value");
			}
		};
		sem_close(sem);
	}

	~NamedSemaphore() {
		sem_close(sem);
		sem_unlink(sem_name.c_str());
	}
private:
	std::string sname;
	sem_t* sem;
	int sem_value;
};

/*
	Usage:
		NamedSemaphore ns;
	at the beginning of main().
*/
