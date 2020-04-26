#include <boost/mpi.hpp>
#include <iostream>
#include <string>
#include <boost/serialization/string.hpp>
//#include <boost/format.hpp>

namespace mpi = boost::mpi;

int main() {
  mpi::environment env;
  mpi::communicator world;

  // Exercise 1, simple hello world
  if (world.rank() == 0) std::cout << "Exercise 1\n\n";
  if (world.rank() != 0) {
    std::string message = "Hello process " + std::to_string(world.rank()) + "!";
    world.send(0, 0, message);
  } else {
    std::cout << "Hello root process!\n";
    for (int source = 1; source < world.size(); source++) {
      std::string message;
      world.recv(source, 0, message);
      std::cout << message << std::endl;
    }
    std::cout.flush();
  }

  //Exercise 2, use wildcards
  if (world.rank() == 0) std::cout << "\n\nExercise 2\n\n";
  if (world.rank() != 0) {
    std::string message = "Hello process " + std::to_string(world.rank()) + "!";
    world.send(0, 0, message);
  } else {
    std::cout << "Hello root process!\n";
    for (int source = 1; source < world.size(); source++) {
      std::string message;
      world.recv(mpi::any_source, mpi::any_tag, message);
      std::cout << message << std::endl;
    }
    std::cout.flush();
  }

  // Exercise 3, send to process p-1
  if (world.rank() == 0) std::cout << "\n\nExercise 3\n\n";
  if (world.rank() != world.size() - 1) {
    std::string message = "Hello process " + std::to_string(world.rank()) + "!";
    world.send(world.size() - 1, 0, message);
  } else {
    std::cout << "Hello root process p-1!\n";
    for (int source = 1; source < world.size() ; source++) {
      std::string message;
      world.recv(mpi::any_source, mpi::any_tag, message);
      std::cout << message << std::endl;
    }
    std::cout.flush();
  }
  return 0;
}