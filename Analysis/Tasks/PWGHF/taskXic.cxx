// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file taskXic.cxx
/// \brief Xic± analysis task
/// \note Inspired from taskLc.cxx
///
/// \author Mattia Faggin <mattia.faggin@cern.ch>, University and INFN PADOVA

#include "Framework/AnalysisTask.h"
#include "Framework/HistogramRegistry.h"
#include "AnalysisDataModel/HFSecondaryVertex.h"
#include "AnalysisDataModel/HFCandidateSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::aod::hf_cand_prong3;
using namespace o2::framework::expressions;

void customize(std::vector<o2::framework::ConfigParamSpec>& workflowOptions)
{
  ConfigParamSpec optionDoMC{"doMC", VariantType::Bool, false, {"Fill MC histograms."}};
  workflowOptions.push_back(optionDoMC);
}

#include "Framework/runDataProcessing.h"

/// Xic± analysis task
struct TaskXic {
  HistogramRegistry registry{
    "registry",
    {{"hmass", "3-prong candidates;inv. mass (p K #pi) (GeV/#it{c}^{2});entries", {HistType::kTH1F, {{500, 1.6, 3.1}}}},
     {"hptcand", "3-prong candidates;candidate #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hptprong0", "3-prong candidates;prong 0 #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hptprong1", "3-prong candidates;prong 1 #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hptprong2", "3-prong candidates;prong 2 #it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hdeclength", "3-prong candidates;decay length (cm);entries", {HistType::kTH1F, {{200, 0., 2.}}}},
     {"hd0Prong0", "3-prong candidates;prong 0 DCAxy to prim. vertex (cm);entries", {HistType::kTH1F, {{100, -1., 1.}}}},
     {"hd0Prong1", "3-prong candidates;prong 1 DCAxy to prim. vertex (cm);entries", {HistType::kTH1F, {{100, -1., 1.}}}},
     {"hd0Prong2", "3-prong candidates;prong 1 DCAxy to prim. vertex (cm);entries", {HistType::kTH1F, {{100, -1., 1.}}}},
     {"hCt", "3-prong candidates;proper lifetime (#Lambda_{c}) * #it{c} (cm);entries", {HistType::kTH1F, {{120, -20., 100.}}}},
     {"hCPA", "3-prong candidates;cosine of pointing angle;entries", {HistType::kTH1F, {{110, -1.1, 1.1}}}},
     {"hEta", "3-prong candidates;candidate #it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}},
     {"hselectionstatus", "3-prong candidates;selection status;entries", {HistType::kTH1F, {{5, -0.5, 4.5}}}},
     {"hImpParErr", "3-prong candidates;impact parameter error (cm);entries", {HistType::kTH1F, {{100, -1., 1.}}}},
     {"hDecLenErr", "3-prong candidates;decay length error (cm);entries", {HistType::kTH1F, {{100, 0., 1.}}}},
     {"hdca2", "3-prong candidates;prong DCA to sec. vertex (cm);entries", {HistType::kTH1F, {{100, 0., 1.}}}}}};

  Configurable<int> d_selectionFlagXic{"d_selectionFlagXic", 1, "Selection Flag for Xic"};
  Configurable<double> cutEtaCandMax{"cutEtaCandMax", -1., "max. cand. pseudorapidity"};

  Filter filterSelectCandidates = (aod::hf_selcandidate_xic::isSelXicpKpi >= d_selectionFlagXic || aod::hf_selcandidate_xic::isSelXicpiKp >= d_selectionFlagXic);

  //void process(aod::HfCandProng3 const& candidates)
  void process(soa::Filtered<soa::Join<aod::HfCandProng3, aod::HFSelXicpKpiCandidate>> const& candidates)
  {
    for (auto& candidate : candidates) {
      if (!(candidate.hfflag() & 1 << XicToPKPi)) {
        continue;
      }
      if (cutEtaCandMax >= 0. && std::abs(candidate.eta()) > cutEtaCandMax) {
        //Printf("Candidate: eta rejection: %g", candidate.eta());
        continue;
      }
      if (candidate.isSelXicpKpi() >= d_selectionFlagXic) {
        registry.fill(HIST("hmass"), InvMassXicpKpi(candidate));
      }
      if (candidate.isSelXicpiKp() >= d_selectionFlagXic) {
        registry.fill(HIST("hmass"), InvMassXicpiKp(candidate));
      }
      registry.fill(HIST("hptcand"), candidate.pt());
      registry.fill(HIST("hptprong0"), candidate.ptProng0());
      registry.fill(HIST("hptprong1"), candidate.ptProng1());
      registry.fill(HIST("hptprong2"), candidate.ptProng2());
      registry.fill(HIST("hdeclength"), candidate.decayLength());
      registry.fill(HIST("hd0Prong0"), candidate.impactParameter0());
      registry.fill(HIST("hd0Prong1"), candidate.impactParameter1());
      registry.fill(HIST("hd0Prong2"), candidate.impactParameter2());
      registry.fill(HIST("hCt"), CtXic(candidate));
      registry.fill(HIST("hCPA"), candidate.cpa());
      registry.fill(HIST("hEta"), candidate.eta());
      registry.fill(HIST("hselectionstatus"), candidate.isSelXicpKpi());
      registry.fill(HIST("hselectionstatus"), candidate.isSelXicpiKp());
      registry.fill(HIST("hImpParErr"), candidate.errorImpactParameter0());
      registry.fill(HIST("hImpParErr"), candidate.errorImpactParameter1());
      registry.fill(HIST("hImpParErr"), candidate.errorImpactParameter2());
      registry.fill(HIST("hDecLenErr"), candidate.errorDecayLength());
      registry.fill(HIST("hDecLenErr"), candidate.chi2PCA());
    }
  }
};

/// Fills MC histograms.
struct TaskXicMC {
  HistogramRegistry registry{
    "registry",
    {{"hPtRecSig", "3-prong candidates (rec. matched);#it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hPtRecBg", "3-prong candidates (rec. unmatched);#it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hPtGen", "3-prong candidates (gen. matched);#it{p}_{T} (GeV/#it{c});entries", {HistType::kTH1F, {{100, 0., 10.}}}},
     {"hCPARecSig", "3-prong candidates (rec. matched);cosine of pointing angle;entries", {HistType::kTH1F, {{110, -1.1, 1.1}}}},
     {"hCPARecBg", "3-prong candidates (rec. unmatched);cosine of pointing angle;entries", {HistType::kTH1F, {{110, -1.1, 1.1}}}},
     {"hEtaRecSig", "3-prong candidates (rec. matched);#it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}},
     {"hEtaRecBg", "3-prong candidates (rec. unmatched);#it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}},
     {"hEtaGen", "3-prong candidates (gen. matched);#it{#eta};entries", {HistType::kTH1F, {{100, -2., 2.}}}}}};

  Configurable<int> d_selectionFlagXic{"d_selectionFlagXic", 1, "Selection Flag for Xic"};
  Configurable<int> d_selectionFlagXicbar{"d_selectionFlagXicbar", 1, "Selection Flag for Xicbar"};
  Configurable<double> cutEtaCandMax{"cutEtaCandMax", -1., "max. cand. pseudorapidity"};

  Filter filterSelectCandidates = (aod::hf_selcandidate_xic::isSelXicpKpi >= d_selectionFlagXic || aod::hf_selcandidate_xic::isSelXicpiKp >= d_selectionFlagXic);

  void process(soa::Filtered<soa::Join<aod::HfCandProng3, aod::HFSelXicpKpiCandidate, aod::HfCandProng3MCRec>> const& candidates,
               soa::Join<aod::McParticles, aod::HfCandProng3MCGen> const& particlesMC)
  {
    // MC rec.
    //Printf("MC Candidates: %d", candidates.size());
    for (auto& candidate : candidates) {
      if (!(candidate.hfflag() & 1 << XicToPKPi)) {
        continue;
      }
      if (cutEtaCandMax >= 0. && std::abs(candidate.eta()) > cutEtaCandMax) {
        //Printf("MC Rec.: eta rejection: %g", candidate.eta());
        continue;
      }
      if (std::abs(candidate.flagMCMatchRec()) == XicToPKPi) {
        registry.fill(HIST("hPtRecSig"), candidate.pt());
        registry.fill(HIST("hCPARecSig"), candidate.cpa());
        registry.fill(HIST("hEtaRecSig"), candidate.eta());
      } else {
        registry.fill(HIST("hPtRecBg"), candidate.pt());
        registry.fill(HIST("hCPARecBg"), candidate.cpa());
        registry.fill(HIST("hEtaRecBg"), candidate.eta());
      }
    }
    // MC gen.
    //Printf("MC Particles: %d", particlesMC.size());
    for (auto& particle : particlesMC) {
      if (cutEtaCandMax >= 0. && std::abs(particle.eta()) > cutEtaCandMax) {
        //Printf("MC Gen.: eta rejection: %g", particle.eta());
        continue;
      }
      if (std::abs(particle.flagMCMatchGen()) == XicToPKPi) {
        registry.fill(HIST("hPtGen"), particle.pt());
        registry.fill(HIST("hEtaGen"), particle.eta());
      }
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{
    adaptAnalysisTask<TaskXic>("hf-task-xic")};
  const bool doMC = cfgc.options().get<bool>("doMC");
  if (doMC) {
    workflow.push_back(adaptAnalysisTask<TaskXicMC>("hf-task-xic-mc"));
  }
  return workflow;
}
