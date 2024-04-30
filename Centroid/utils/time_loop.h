
#ifndef TIMELOOP
#define TIMELOOP

template<class F, class G, class H>
void time_loop(int rounds, double delay, F initf, G runf, H endf) {
  // run for delay seconds to "warm things up"
  // will skip if delay is zero
  double total_time = 0.0;
  // while (t.total_time() < delay) {
  //   initf(); runf(); endf();
  // } 
  for (int i=0; i < rounds; i++) {
    parlay::internal::timer t;
    initf();
    t.start();
    runf();
    t.stop(); total_time += t.total_time();
    endf();
  }
  std::cout << "# time per iter: " << total_time/rounds << std::endl;
}

#endif
