#include "queueing_system_gui.h"
#include "queueing_system_research.h"

#include <imgui.h>
#include <implot.h>

#include <vector>
#include <memory>
#include <array>
#include <charconv>

namespace QS = QueueingSystem;
namespace QSGui = QueueingSystemGui;

void QSGui::sourcesStatusTable(const std::vector<QS::USourceStatus>& sourcesStatus)
{
    if (ImGui::BeginTable("SourcesStats", 4, tableFlags))
    {
        ImGui::TableSetupColumn(u8"#");
        ImGui::TableSetupColumn(u8"Время");
        ImGui::TableSetupColumn(u8"Заявки");
        ImGui::TableSetupColumn(u8"Отказы");
        ImGui::TableHeadersRow();

        for (int i{}; i < sourcesStatus.size(); ++i)
        {
            const auto& sourceStatus{ sourcesStatus[i] };

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text(u8"И%d", i);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", *(sourceStatus->nextGenerationTime));

            ImGui::TableNextColumn();
            ImGui::Text("%d", *(sourceStatus->requestsCount));

            ImGui::TableNextColumn();
            ImGui::Text("%d", *(sourceStatus->rejectionsCount));
        }

        ImGui::EndTable();
    }
}

void QSGui::devicesStatusTable(const std::vector<QS::UDeviceStatus>& devicesStatus)
{
    if (ImGui::BeginTable("DevicesStats", 4, tableFlags))
    {
        ImGui::TableSetupColumn(u8"#");
        ImGui::TableSetupColumn(u8"Время");
        ImGui::TableSetupColumn(u8"Заявки");
        ImGui::TableSetupColumn(u8"Состояние");
        ImGui::TableHeadersRow();

        for (int i{}; i < devicesStatus.size(); ++i)
        {
            const auto& deviceStatus{ devicesStatus[i] };

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text(u8"П%d", i);

            ImGui::TableNextColumn();
            *(deviceStatus->processingEndTime) < 0.0 ? ImGui::Text("-") :
                ImGui::Text("%.3f", *(deviceStatus->processingEndTime));

            ImGui::TableNextColumn();
            ImGui::Text("%d", *(deviceStatus->requestsCount));

            ImGui::TableNextColumn();
            ImGui::Text("%s", *(deviceStatus->processingEndTime) < 0.0 ? u8"свободен" : u8"занят");
        }

        ImGui::EndTable();
    }
}

void QSGui::bufferStatusTable(const QS::URequest* buffer, int bufferSize)
{
    static const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    if (ImGui::BeginTable("Buffer", bufferSize + 1, tableFlags | ImGuiTableFlags_ScrollX, ImVec2(0.0f, TEXT_BASE_HEIGHT * 4)))
    {
        ImGui::TableSetupScrollFreeze(1, 0);

        static std::array<char, 4> header{ '\0' };
        ImGui::TableSetupColumn("data", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        for (int positionNumber{}; positionNumber < bufferSize; ++positionNumber)
        {
            std::to_chars(header.data(), header.data() + header.size(), positionNumber);
            ImGui::TableSetupColumn(header.data(), ImGuiTableColumnFlags_WidthFixed, 70.0f);
        }

        auto color{ ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg)) };

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(u8"Позиция");
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
        for (int positionNumber{}; positionNumber < bufferSize; ++positionNumber)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%d", positionNumber);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(u8"Время");
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
        for (int i{}; i < bufferSize; ++i)
        {
            ImGui::TableNextColumn();
            const auto& position{ buffer[i] };
            position ? ImGui::Text("%.3f", position->generationTime) : ImGui::Text("-");
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(u8"Заявка");
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
        for (int i{}; i < bufferSize; ++i)
        {
            ImGui::TableNextColumn();
            const auto& position{ buffer[i] };
            position ? ImGui::Text("(%d, %d)", position->id.sourceId, position->id.serialNumber) : ImGui::Text("-");
        }

        ImGui::EndTable();
    }
}

void QSGui::sourcesResultsTable(const std::vector<QS::USourceFinalStats>& sourcesFinalStats)
{
    if (ImGui::BeginTable("SourcesFinalStats", 8, tableFlags))
    {
        ImGui::TableSetupColumn(u8"#");
        ImGui::TableSetupColumn(u8"Заявки");
        ImGui::TableSetupColumn(u8"P отк");
        ImGui::TableSetupColumn(u8"T преб");
        ImGui::TableSetupColumn(u8"T БП");
        ImGui::TableSetupColumn(u8"T обсл");
        ImGui::TableSetupColumn(u8"Д БП");
        ImGui::TableSetupColumn(u8"Д обсл");
        ImGui::TableHeadersRow();

        for (int row{}; row < sourcesFinalStats.size(); ++row)
        {
            ImGui::TableNextRow();

            const auto& sourceFinalStats{ *sourcesFinalStats[row] };

            ImGui::TableNextColumn();
            ImGui::Text(u8"И%d", row);

            ImGui::TableNextColumn();
            ImGui::Text("%d", sourceFinalStats.requestsCount);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", sourceFinalStats.rejectionProbability);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", sourceFinalStats.averageProcessingTime);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", sourceFinalStats.averageBufferTime);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", sourceFinalStats.averageServiceTime);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", sourceFinalStats.bufferTimeDispersion);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", sourceFinalStats.serviceTimeDispersion);
        }

        ImGui::EndTable();
    }
}

void QSGui::devicesResultsTable(const std::vector<QS::UDeviceFinalStats>& devicesFinalStats)
{
    if (ImGui::BeginTable("SourcesFinalStats", 4, tableFlags))
    {
        ImGui::TableSetupColumn(u8"#");
        ImGui::TableSetupColumn(u8"Заявки");
        ImGui::TableSetupColumn(u8"T обсл");
        ImGui::TableSetupColumn(u8"Коэф. использования");
        ImGui::TableHeadersRow();

        for (int row{}; row < devicesFinalStats.size(); ++row)
        {
            ImGui::TableNextRow();

            const auto& deviceFinalStats{ *devicesFinalStats[row] };

            ImGui::TableNextColumn();
            ImGui::Text(u8"П%d", row);

            ImGui::TableNextColumn();
            ImGui::Text("%d", deviceFinalStats.requestsCount);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", deviceFinalStats.averageServiceTime);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", deviceFinalStats.utilizationFactor);
        }

        ImGui::EndTable();
    }
}

void QSGui::configuration(QS::QueueingSystem& system, QS::SystemConfiguration& conf, QS::SystemStatus& status)
{
    ImGui::Begin(u8"Конфигурация системы");

    ImGui::Text(u8"Количество источников: %d", conf.sourcesCount);
    ImGui::Text(u8"Интервал распределения времени генерации: 0.0 - %.3f", conf.distrRange);
    ImGui::Text(u8"Размер буфера: %d", conf.bufferSize);
    ImGui::Text(u8"Количество приборов: %d", conf.devicesCount);
    ImGui::Text(u8"Лямбда эскпоненциального распределения: %.3f", conf.lambda);
    ImGui::Text(u8"Макс. количество заявок: %d", conf.requestsLimit);

    static bool configCange{ false };
    if (ImGui::Button(u8"Изменить"))
        configCange = true;

    if (configCange)
        configurationChanges(system, conf, status, configCange);

    ImGui::SameLine();

    static bool research{ false };
    if (ImGui::Button(u8"Исследовать"))
        research = true;

    if (research)
        researchSystem(system, research);

    ImGui::End();
}

void QSGui::configurationChanges(QS::QueueingSystem& system, QS::SystemConfiguration& conf,
    QS::SystemStatus& status, bool& configChange)
{
    ImGui::Begin(u8"Изменение конфигурации системы", &configChange);

    static int sourcesCount{ conf.sourcesCount };
    ImGui::SliderInt(u8"Количество источников", &sourcesCount, 1, 100, "%d", sliderFlags);

    static float distrRange{ conf.distrRange };
    ImGui::SliderFloat(u8"Интервал распределения времени генерации: ", &distrRange, 1.0, 30.0, "%.3f", sliderFlags);

    static int bufferSize{ conf.bufferSize };
    ImGui::SliderInt(u8"Размер буфера", &bufferSize, 1, 500, "%d", sliderFlags);

    static int devicesCount{ conf.devicesCount };
    ImGui::SliderInt(u8"Количество приборов", &devicesCount, 1, 10000, "%d", sliderFlags);

    static float lambda{ conf.lambda };
    ImGui::SliderFloat(u8"Лямбда эскпоненциального распределения", &lambda, 0.001, 1.0, "%.3f", sliderFlags);

    static int requestsLimit{ conf.requestsLimit };
    ImGui::SliderInt(u8"Макс. количество заявок", &requestsLimit, 1, 100000, "%d", sliderFlags);

    if (ImGui::Button(u8"Применить"))
    {
        conf.sourcesCount = sourcesCount;
        conf.distrRange = distrRange;
        conf.bufferSize = bufferSize;
        conf.devicesCount = devicesCount;
        conf.lambda = lambda;
        conf.requestsLimit = requestsLimit;

        system.reset(conf);
        status = system.getSystemStatus();
    }

    ImGui::SameLine();

    if (ImGui::Button(u8"Отмена"))
    {
        sourcesCount = conf.sourcesCount;
        distrRange = conf.distrRange;
        bufferSize = conf.bufferSize;
        devicesCount = conf.devicesCount;
        lambda = conf.lambda;
        requestsLimit = conf.requestsLimit;
    }

    ImGui::End();
}

void QSGui::researchSystem(QS::QueueingSystem& system, bool& research)
{
    ImGui::Begin(u8"Исследование СМО", &research);

    static QS::ResearchedConfStats sysConfs{};
    static bool showResearchedResults{};

    static QS::GraphicsData varyDevCountGraphics{};
    static QS::GraphicsData varyLambdaGraphics{};
    static QS::GraphicsData varyBufferSizeGraphics{};

    if (!showResearchedResults && ImGui::Button(u8"Старт"))
    {
        sysConfs = QS::researchQueueingSystem(10, 5.0);//аналогично для (5, 2.5) и (15, 7.5)
        showResearchedResults = true;

        const auto& bestConf{ **sysConfs.cbegin() };
        int fixedBufferSize{ bestConf.conf->bufferSize };
        int fixedDevicesCount{ bestConf.conf->devicesCount };
        float fixedLamda{ bestConf.conf->lambda };

        varyDevCountGraphics = QS::getGraphicsDataVaryDevicesCount(fixedBufferSize, fixedLamda);
        varyLambdaGraphics = QS::getGraphicsDataVaryLambda(fixedBufferSize, fixedDevicesCount);
        varyBufferSizeGraphics = QS::getGraphicsDataVaryBufferSize(fixedDevicesCount, fixedLamda);
    }

    if (showResearchedResults && ImGui::Button(u8"Сброс"))
    {
        showResearchedResults = false;
        sysConfs.clear();

        varyDevCountGraphics.first.clear();
        varyDevCountGraphics.second.first.clear();
        varyDevCountGraphics.second.second.clear();

        varyLambdaGraphics.first.clear();
        varyLambdaGraphics.second.first.clear();
        varyLambdaGraphics.second.second.clear();

        varyBufferSizeGraphics.first.clear();
        varyBufferSizeGraphics.second.first.clear();
        varyBufferSizeGraphics.second.second.clear();
    }

    if (showResearchedResults)
    {
        ImGui::SeparatorText(u8"Исследование при 10 истоничках");
        bestConfigurationsTable(sysConfs);

        if (ImPlot::BeginPlot(u8"Зависимость вероятности отказа от количества приборов", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0)))
        {
            ImPlot::SetupAxes(u8"Количество приборов", u8"Вероятность отказа");
            ImPlot::PlotLine("", varyDevCountGraphics.first.data(),
                varyDevCountGraphics.second.first.data(), varyDevCountGraphics.first.size());
            ImPlot::EndPlot();
        }

        ImGui::SameLine();

        if (ImPlot::BeginPlot(u8"Зависимость загруженности системы от количества приборов"))
        {
            ImPlot::SetupAxes(u8"Количество приборов", u8"Загруженность системы");
            ImPlot::PlotLine("", varyDevCountGraphics.first.data(),
                varyDevCountGraphics.second.second.data(), varyDevCountGraphics.first.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot(u8"Зависимость вероятности отказа от лямбда", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0)))
        {
            ImPlot::SetupAxes(u8"Лямбда", u8"Вероятность отказа");
            ImPlot::PlotLine("", varyLambdaGraphics.first.data(),
                varyLambdaGraphics.second.first.data(), varyLambdaGraphics.first.size());
            ImPlot::EndPlot();
        }

        ImGui::SameLine();

        if (ImPlot::BeginPlot(u8"Зависимость загруженности системы от лямбда"))
        {
            ImPlot::SetupAxes(u8"Лямбда", u8"Загруженность системы");
            ImPlot::PlotLine("", varyLambdaGraphics.first.data(),
                varyLambdaGraphics.second.second.data(), varyLambdaGraphics.first.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot(u8"Зависимость вероятности отказа от размера буфера", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0)))
        {
            ImPlot::SetupAxes(u8"Размер буфера", u8"Вероятность отказа");
            ImPlot::PlotLine("", varyBufferSizeGraphics.first.data(),
                varyBufferSizeGraphics.second.first.data(), varyBufferSizeGraphics.first.size());
            ImPlot::EndPlot();
        }

        ImGui::SameLine();

        if (ImPlot::BeginPlot(u8"Зависимость загруженности системы от размера буфера"))
        {
            ImPlot::SetupAxes(u8"Размер буфера", u8"Загруженность системы");
            ImPlot::PlotLine("", varyBufferSizeGraphics.first.data(),
                varyBufferSizeGraphics.second.second.data(), varyBufferSizeGraphics.first.size());
            ImPlot::EndPlot();
        }
    }

    ImGui::End();
}

void QSGui::bestConfigurationsTable(const QS::ResearchedConfStats& data)
{
    if (ImGui::BeginTable("BestConfigurations", 8, tableFlags))
    {
        ImGui::TableSetupColumn("#");
        ImGui::TableSetupColumn("nSources");
        ImGui::TableSetupColumn("[a, b]");
        ImGui::TableSetupColumn("BufSize");
        ImGui::TableSetupColumn("nDevices");
        ImGui::TableSetupColumn("lambda");
        ImGui::TableSetupColumn("P_rej");
        ImGui::TableSetupColumn("Workload");
        ImGui::TableHeadersRow();

        static int counter{};
        for (const auto& point : data)
        {
            ++counter;

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text(u8"%d", counter);

            ImGui::TableNextColumn();
            ImGui::Text("%d", point->conf->sourcesCount);

            ImGui::TableNextColumn();
            ImGui::Text("[0.0, %.3f]", point->conf->distrRange);

            ImGui::TableNextColumn();
            ImGui::Text("%d", point->conf->bufferSize);

            ImGui::TableNextColumn();
            ImGui::Text("%d", point->conf->devicesCount);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", point->conf->lambda);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", point->rejectionProbability);

            ImGui::TableNextColumn();
            ImGui::Text("%.3f", point->workload);

            if (counter == 10)
            {
                counter = 0;
                break;
            }
        }

        ImGui::EndTable();
    }
}

void QSGui::controls(QS::QueueingSystem& system, bool& showResultsWindow, QS::USystemFinalStats& finalStats)
{
    ImGui::Begin(u8"Управление");

    //auto sz = ImVec2(-FLT_MIN, 0.0f);

    if (ImGui::Button(u8"Шаг"))
    {
        if (!showResultsWindow && !system.makeStep())
        {
            finalStats = std::make_unique<QS::SystemFinalStats>(system.getSystemFinalStats());
            showResultsWindow = true;
        }
    }
    ImGui::SameLine();
    ImGui::Text(u8"- пошаговый режим");
    ImGui::Spacing();

    if (ImGui::Button(u8"Авто"))
    {
        while (!showResultsWindow && system.makeStep());
        finalStats = std::make_unique<QS::SystemFinalStats>(system.getSystemFinalStats());
        showResultsWindow = true;
    }
    ImGui::SameLine();
    ImGui::Text(u8"- автоматический режим");
    ImGui::Spacing();

    if (ImGui::Button(u8"Сброс"))
    {
        system.reset();
        showResultsWindow = false;
    }
    ImGui::SameLine();
    ImGui::Text(u8"- сброс в изначальное состояние");
    ImGui::Spacing();

    ImGui::End();
}

void QSGui::stepStatistics(const QS::SystemStatus& systemStatus)
{
    ImGui::Begin(u8"Пошаговая статистика");

    ImGui::BeginChild("SourcesStatus",
        ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 300));
    ImGui::SeparatorText(u8"Источники");
    QSGui::sourcesStatusTable(systemStatus.sourcesStatus);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("DevicesStatus", ImVec2(0, 300));
    ImGui::SeparatorText(u8"Приборы");
    QSGui::devicesStatusTable(systemStatus.devicesStatus);
    ImGui::EndChild();

    ImGui::SeparatorText(u8"Буфер");
    QSGui::bufferStatusTable(systemStatus.bufferStatus->buffer, systemStatus.bufferStatus->size);

    ImGui::Spacing();
    ImGui::Text(u8"Прогресс: %d из %d", *systemStatus.requestsCountStatus->requestCount,
        systemStatus.requestsCountStatus->requestsLimit);
    ImGui::ProgressBar(static_cast<float>(*systemStatus.requestsCountStatus->requestCount)
        / systemStatus.requestsCountStatus->requestsLimit);

    ImGui::End();
}

void QSGui::finalStatistics(const QS::SystemFinalStats& finalStats, bool& showReslutsWindow)
{
    ImGui::Begin(u8"Результаты", &showReslutsWindow);

    ImGui::BeginChild("SourcesResults",
        ImVec2(ImGui::GetContentRegionAvail().x * 0.65f, 300));
    ImGui::SeparatorText(u8"Характеристики источников ВС");
    QSGui::sourcesResultsTable(finalStats.sourcesFinalStats);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("DevicesResults", ImVec2(0, 300));
    ImGui::SeparatorText(u8"Характеристики приборов ВС");
    QSGui::devicesResultsTable(finalStats.deviceFinalStats);
    ImGui::EndChild();

    ImGui::Text(u8"Вероятность отказа заявкам в обслуживании: %.3f",
        finalStats.rejectionProbability);
    ImGui::Text(u8"Загрузка системы: %.3f",
        finalStats.workload);
    ImGui::Text(u8"Необходимое количество заявок: %d", finalStats.requiredRequestsCount);

    ImGui::End();
}
