#include "queueing_system_research.h"

namespace QS = QueueingSystem;

bool QS::confSatisfyConstraints(const SystemFinalStats& stats)
{
    return stats.rejectionProbability < REJECT_PROB_CONSTRAINT && stats.workload > WORKLOAD_CONSTRAINT;
}

bool QS::USysConfStatsCmp(const USysConfStats& left, const USysConfStats& right)
{
    return (left->workload > right->workload) ||
        (left->workload == right->workload && left->conf->devicesCount < right->conf->devicesCount) ||
        (left->workload == right->workload && left->conf->devicesCount == right->conf->devicesCount &&
         left->conf->bufferSize < right->conf->bufferSize);
}

QS::ResearchedConfStats QS::researchQueueingSystem(int sourcesCount, float distrRange)
{
    ResearchedConfStats sysConfigurations{ &USysConfStatsCmp };

    auto sysConf{ std::make_unique<SystemConfiguration>() };
    sysConf->sourcesCount = sourcesCount;
    sysConf->distrRange = distrRange;

    auto system{ std::make_unique<QueueingSystem>(*sysConf) };

    USystemFinalStats finalStats{};

    for (int bufferSize{ 1 }; bufferSize < 51; ++bufferSize)//1 - 51
    {
        for (int devicesCount{ 1 }; devicesCount < 51; ++devicesCount)
        {
            for (int lambda{ 1 }; lambda < 51; ++lambda)//0 - 51
            {
                sysConf->bufferSize = bufferSize * 10;
                sysConf->devicesCount = devicesCount * 10;
                sysConf->lambda = 0.02f + lambda * 0.001f;
                //sysConf->lambda = 0.05f;
                system->reset(*sysConf);

                while (system->makeStep());
                finalStats = std::make_unique<QS::SystemFinalStats>(system->getSystemFinalStats());

                if (confSatisfyConstraints(*finalStats))
                {
                    sysConfigurations.insert(std::make_unique<SystemConfigurationStats>(
                        SystemConfigurationStats
                        {
                            std::make_unique<SystemConfiguration>(*sysConf),
                            finalStats->rejectionProbability,
                            finalStats->workload
                        }
                    ));
                }
            }
        }
    }

    return sysConfigurations;
}

QS::GraphicsData QS::getGraphicsDataVaryDevicesCount(int bufferSize, float lambda)
{
    auto sysConf{ std::make_unique<SystemConfiguration>() };
    sysConf->bufferSize = bufferSize;
    sysConf->lambda = lambda;

    auto system{ std::make_unique<QueueingSystem>(*sysConf) };

    USystemFinalStats finalStats{};

    std::vector<float> dataX{};
    std::vector<float> dataYRejProb{};
    std::vector<float> dataYWorkload{};

    for (int devicesCount{ 1 }; devicesCount < 1001; ++devicesCount)
    {
        sysConf->devicesCount = devicesCount;
        system->reset(*sysConf);

        while (system->makeStep());
        finalStats = std::make_unique<QS::SystemFinalStats>(system->getSystemFinalStats());

        dataX.push_back(sysConf->devicesCount);
        dataYRejProb.push_back(finalStats->rejectionProbability);
        dataYWorkload.push_back(finalStats->workload);
    }

    return std::make_pair(dataX, std::make_pair(dataYRejProb, dataYWorkload));
}

QS::GraphicsData QS::getGraphicsDataVaryLambda(int bufferSize, int devicesCount)
{
    auto sysConf{ std::make_unique<SystemConfiguration>() };
    sysConf->bufferSize = bufferSize;
    sysConf->devicesCount = devicesCount;

    auto system{ std::make_unique<QueueingSystem>(*sysConf) };

    USystemFinalStats finalStats{};

    std::vector<float> dataX{};
    std::vector<float> dataYRejProb{};
    std::vector<float> dataYWorkload{};

    for (int lambda{ 1 }; lambda < 1001; ++lambda)
    {
        sysConf->lambda = lambda * 0.0001;
        system->reset(*sysConf);

        while (system->makeStep());
        finalStats = std::make_unique<QS::SystemFinalStats>(system->getSystemFinalStats());

        dataX.push_back(sysConf->lambda);
        dataYRejProb.push_back(finalStats->rejectionProbability);
        dataYWorkload.push_back(finalStats->workload);
    }

    return std::make_pair(dataX, std::make_pair(dataYRejProb, dataYWorkload));
}

QS::GraphicsData QS::getGraphicsDataVaryBufferSize(int devicesCount, float lambda)
{
    auto sysConf{ std::make_unique<SystemConfiguration>() };
    sysConf->devicesCount = devicesCount;
    sysConf->lambda = lambda;

    auto system{ std::make_unique<QueueingSystem>(*sysConf) };

    USystemFinalStats finalStats{};

    std::vector<float> dataX{};
    std::vector<float> dataYRejProb{};
    std::vector<float> dataYWorkload{};

    for (int bufferSize{ 1 }; bufferSize < 501; ++bufferSize)
    {
        sysConf->bufferSize = bufferSize;
        system->reset(*sysConf);

        while (system->makeStep());
        finalStats = std::make_unique<QS::SystemFinalStats>(system->getSystemFinalStats());

        dataX.push_back(sysConf->bufferSize);
        dataYRejProb.push_back(finalStats->rejectionProbability);
        dataYWorkload.push_back(finalStats->workload);
    }

    return std::make_pair(dataX, std::make_pair(dataYRejProb, dataYWorkload));
}
