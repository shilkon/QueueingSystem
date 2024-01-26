#ifndef QUEUEING_SYSTEM_GUI_H
#define QUEUEING_SYSTEM_GUI_H

#include "queueing_system.h"
#include "queueing_system_research.h"

#include <imgui.h>

#include <vector>
#include <memory>
#include <cassert>

namespace QueueingSystemGui
{
    namespace QS = QueueingSystem;

    inline constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders;
    inline constexpr ImGuiTableFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

    void sourcesStatusTable(const std::vector<QS::USourceStatus>& sourcesStatus);
    void devicesStatusTable(const std::vector<QS::UDeviceStatus>& devicesStatus);
    void bufferStatusTable(const QS::URequest* bufferStart, int bufferSize);

    void sourcesResultsTable(const std::vector<QS::USourceFinalStats>& sourcesFinalStats);
    void devicesResultsTable(const std::vector<QS::UDeviceFinalStats>& devicesFinalStats);

    void configuration(QS::QueueingSystem& system, QS::SystemConfiguration& conf,
        QS::SystemStatus& status);
    void configurationChanges(QS::QueueingSystem& system, QS::SystemConfiguration& conf,
        QS::SystemStatus& status, bool& configChange);

    void researchSystem(QS::QueueingSystem& system, bool& research);
    void bestConfigurationsTable(const QS::ResearchedConfStats& data);

    void controls(QS::QueueingSystem& system, bool& showResultsWindow, QS::USystemFinalStats& finalStats);
    void stepStatistics(const QS::SystemStatus& systemStatus);
    void finalStatistics(const QS::SystemFinalStats& finalStats, bool& showReslutsWindow);

}

#endif
