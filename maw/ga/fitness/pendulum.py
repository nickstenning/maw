class Evaluator(object):
    pass

# // Pendulum config
# BANG_SIZE     10.0
# NOISE_LEVEL   0.0
# MAX_EVAL_TIME 100.0
# PI            3.141592653589793238462643
# SCORE_ANG     (PI - 0.1)


# /**
#  * Feed the state variables into the brain, feed forward, and advance the
#  * physics using the output of the brain as a controlling torque.
# **/
# double PendulumFitnessFunction::operator() (Evolvable* obj)
# {
#   Brain* brain = dynamic_cast<Brain*>(obj);
#
#   if (brain == NULL) {
#     throw std::runtime_error("PendulumFitnessFunction received an Evolvable instance that wasn't a Brain!");
#   }
#
#   double fitness = 0.0;
#
#   m_pdl.ang(util::rand(-SCORE_ANG, SCORE_ANG))
#   .vel(0)
#   .time(0);
#
#   while (m_pdl.time() < MAX_EVAL_TIME) {
#     step(brain);
#
#     bool in_scoring_zone = std::abs(m_pdl.ang()) < SCORE_ANG;
#     if (in_scoring_zone) {
#       fitness += m_pdl.dt * util::dirac_delta(m_pdl.ang(), 5);
#     } else {
#       break; // Failure. No need to evaluate further.
#     }
#   }
#
#   return fitness;
# }
#
# void PendulumFitnessFunction::step (Brain* brain)
# {
#   std::vector<double> input;
#   std::vector<int> output;
#
#   input.push_back(m_pdl.ang());
#   input.push_back(m_pdl.vel());
#
#   output = brain->feed(input);
#
#   double controlTorque = BANG_SIZE * output[0];
#   double noiseTorque   = util::rand(-NOISE_LEVEL, NOISE_LEVEL);
#
#   m_pdl.step(controlTorque + noiseTorque);
# }