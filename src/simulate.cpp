#include <string>
#include <sstream>
#include <vector>

#include "vendor/zmq.h"

#include "nn.h"
#include "unicycle_2d.h"
#include "zhelpers.h"
#include "util.h"

static const double CONTROLLER_FORCE = 20.0;

double fw = 0.0, fp = 0.0, tgt = 0.0;

std::string process_comm(std::string&, Unicycle2D&, NN& nn);
std::string process_data(std::string&, Unicycle2D&, NN& nn);

int main (int /*argc*/, char* const /*argv*/[]) {
  util::initRNG();

  NN nn;
  std::cin >> nn;

  Unicycle2D uni;

  zmq::context_t ctx(1);

  zmq::socket_t comm(ctx, ZMQ_REP);
  zmq::socket_t data(ctx, ZMQ_REP);

  comm.connect("tcp://127.0.0.1:5000");
  data.connect("tcp://127.0.0.1:5001");

  // Initialize poll set
  zmq::pollitem_t poll_items [] = {
    { comm, 0, ZMQ_POLLIN, 0 },
    { data, 0, ZMQ_POLLIN, 0 }
  };

  // Initialize unicycle;
  uni.p(util::rand(-0.001, 0.001));

  while (1) {
    std::string req, rep;
    zmq::poll(&poll_items[0], 2, -1);

    // Got command
    if (poll_items[0].revents & ZMQ_POLLIN) {
      s_recv(comm, req);
      rep = process_comm(req, uni, nn);
      s_send(comm, rep);
    }

    // Got request for data
    if (poll_items[1].revents & ZMQ_POLLIN) {
      s_recv(data, req);
      rep = process_data(req, uni, nn);
      s_send(data, rep);
    }

    // std::cout << uni.T() << " + " << uni.V() << " = " << uni.T() + uni.V() << "\n";
  }

  return 0;
}

double getControlForce(Unicycle2D& uni, NN& nn) {
  std::vector<double> input;
  std::vector<int> output;

  input.push_back(uni.p());
  input.push_back(uni.dpdt());
  input.push_back(uni.w());
  input.push_back(uni.dwdt());
  input.push_back(tgt);

  output = nn.feedForward(input);

  double controlForce = CONTROLLER_FORCE * output[0];

  return controlForce;
}

std::string process_comm(std::string& msg, Unicycle2D& uni, NN& /*nn*/) {
  std::istringstream s_in( msg );
  std::ostringstream s_out;
  std::string cmd;

  std::cout << "comm: " << msg << "\n";

  s_in >> cmd;

  if (cmd.compare("fw") == 0) {
    s_in >> fw;
  }

  if (cmd.compare("fp") == 0) {
    s_in >> fp;
  }

  if (cmd.compare("tgt") == 0) {
    s_in >> tgt;
  }

  if (cmd.compare("rst") == 0) {
    uni.p(util::rand(-0.001, 0.001))
       .dpdt(0.0)
       .w(0.0)
       .dwdt(0.0);
  }

  s_out << "OK";

  return s_out.str();
}

std::string process_data(std::string& msg, Unicycle2D& uni, NN& nn) {
  std::istringstream s_in( msg );
  std::ostringstream s_out;

  double dt;
  s_in >> dt;

  double controlForce = getControlForce(uni, nn);
  uni.step(dt, controlForce + fw, fp);

  s_out << uni.p() << " " << uni.w() << " " << controlForce + fw << " " << fp;

  return s_out.str();
}



