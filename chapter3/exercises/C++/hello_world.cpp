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
  if (world.rank() != 0) {
    std::string message = "Hello process " + std::to_string(world.rank()) + "!";
    world.send(0, 0, message);
  } else {
    std::cout << "Hello root process!" << std::endl;
    for (int source = 1; source < world.size(); source++) {
      std::string message;
      world.recv(source, 0, message);
      std::cout << message << std::endl;
    }
    std::cout.flush();
  }
  return 0;
}