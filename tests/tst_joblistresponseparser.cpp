#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "joblistresponseparser.h"
#include <QString>
#include <QList>

using namespace jenkinsQViewer;

const char* twoJobsResponse = u8R"(<?xml version="1.0" encoding="UTF-8"?>
<hudson>
    <assignedLabel />
    <mode>NORMAL</mode>
    <nodeDescription>the master Jenkins node</nodeDescription>
    <nodeName />
    <numExecutors>2</numExecutors>
    <job>
        <name>job1</name>
        <url>http://localhost:8080/job/job1/</url>
        <color>red</color>
    </job>
    <job>
        <name>turbo-job2</name>
        <url>http://localhost:8080/job/turbo-job2/</url>
        <color>blue_anime</color>
    </job>
    <overallLoad />
    <primaryView>
        <name>All</name>
        <url>http://localhost:8080/</url>
    </primaryView>
    <quietingDown>false</quietingDown>
    <slaveAgentPort>0</slaveAgentPort>
    <unlabeledLoad />
    <useCrumbs>false</useCrumbs>
    <useSecurity>false</useSecurity>
    <view>
        <name>All</name>
        <url>http://localhost:8080/</url>
    </view>
</hudson>
)";

const char* progressResponse = u8R"(<?xml version="1.0" encoding="UTF-8"?>
<freeStyleBuild>
    <action>
        <cause>
            <shortDescription>Started by user anonymous</shortDescription>
            <userName>anonymous</userName>
        </cause>
    </action>
    <building>true</building>
    <displayName>#11</displayName>
    <duration>0</duration>
    <estimatedDuration>20067</estimatedDuration>
    <executor>
        <currentExecutable>
            <number>11</number>
            <url>http://localhost:8080/job/turbo-job2/11/</url>
        </currentExecutable>
        <currentWorkUnit />
        <idle>false</idle>
        <likelyStuck>false</likelyStuck>
        <number>0</number>
        <progress>38</progress>
    </executor>
    <fullDisplayName>turbo-job2 #11</fullDisplayName>
    <id>11</id>
    <keepLog>false</keepLog>
    <number>11</number>
    <queueId>15</queueId>
    <timestamp>1454972013460</timestamp>
    <url>http://localhost:8080/job/turbo-job2/11/</url>
    <builtOn />
    <changeSet />
</freeStyleBuild>
)";

TEST(JoblistResponseParser, will_emit_new_job)
{
    QList<Job*> jobs;
    JoblistResponseParser parser(jobs);
    QString response = QString::fromUtf8(twoJobsResponse);
    parser.parseJoblistResponse(response);
    EXPECT_EQ(2, jobs.count());
}

TEST(JoblistResponseParser, will_read_red_blue_running_color)
{
    QList<Job*> jobs;
    JoblistResponseParser parser(jobs);
    QString response = QString::fromUtf8(twoJobsResponse);
    parser.parseJoblistResponse(response);
    EXPECT_EQ(2, jobs.count());
    EXPECT_EQ(QColor(Qt::darkRed), jobs.at(0)->getColor());
    EXPECT_EQ(QColor(Qt::darkGreen), jobs.at(1)->getColor());
    EXPECT_FALSE(jobs.at(0)->getRunning());
    EXPECT_TRUE(jobs.at(1)->getRunning());
}

TEST(JoblistResponseParser, will_set_progress)
{
    QList<Job*> jobs;
    Job* job = new Job("turbo-job2");
    jobs.append(job);
    JoblistResponseParser parser(jobs);
    QString response = QString::fromUtf8(progressResponse);
    parser.parseJoblistResponse(response);
    EXPECT_FLOAT_EQ(0.38f, job->getBuildProgress());
}
