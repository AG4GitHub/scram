#include <gtest/gtest.h>

#include "fault_tree_analysis_tests.h"

// Benchmark tests for Lift system from OpenFTA
TEST_F(FaultTreeAnalysisTest, Lift) {
  std::string tree_input = "./share/scram/input/benchmark/lift.scramf";
  std::vector<std::string> events;
  events.push_back("lmd_1");
  events.push_back("dpd_1");
  events.push_back("dm_1");
  events.push_back("ps_1");
  events.push_back("dod_1");
  events.push_back("dod_2");
  events.push_back("dms_1");
  events.push_back("cp_1");
  events.push_back("dms_2");
  events.push_back("lmd_2");
  events.push_back("lpd_1");
  events.push_back("d_1");

  std::set<std::string> cut_set;
  std::set< std::set<std::string> > mcs;  // For expected min cut sets.

  ASSERT_NO_THROW(ran->ProcessInput(tree_input));
  ASSERT_NO_THROW(ran->Analyze());
  ASSERT_NO_THROW(ran->Report("/dev/null"));
  double delta_sqr = std::abs(p_total() - 1.19999e-5);
  EXPECT_TRUE(delta_sqr < 1e-5);
  // Minimal cut set check.
  std::vector<std::string>::iterator it;
  for (it = events.begin(); it != events.end(); ++it) {
    cut_set.insert(*it);
    mcs.insert(cut_set);
    cut_set.clear();
  }
  EXPECT_EQ(12, min_cut_sets().size());
  EXPECT_EQ(mcs, min_cut_sets());
}
