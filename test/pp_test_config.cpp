//
// Created by bluse on 1/13/17.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "log.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// note: please running on valgrid

#if 0

TEST(config,scan)
{
    // read file
    using boost::property_tree::ptree;
    ptree ptroot;
    boost::property_tree::ini_parser::read_ini("./config/pinpoint_agent.config", ptroot);

#define SHOW_KEY(para) \
    LOGI(para" = %s",ptroot.get<std::string>(para).c_str())

    SHOW_KEY("common.CollectorStatIp");
    SHOW_KEY("common.CollectorStatPort");

    try {
        SHOW_KEY("404");
    }
    catch (boost::property_tree::ptree_error  pterror)
    {
        LOGI(" %s ", pterror.what());
        EXPECT_TRUE(true);
    }
}

TEST(config,scan_section)
{
    using boost::property_tree::ptree;
    ptree ptroot;
    boost::property_tree::ini_parser::read_ini("./config/pinpoint_agent.config", ptroot);

    ptree tpt = ptroot.get_child("TEST");
    std::cout<<tpt.get_value("uint64_404");

}


TEST(config,init_and_free)
{

    extern AgentParameter __agentParamter;
    AgentParameter *parameter = NULL;

    parameter = create_agent_para_factory("");
    EXPECT_EQ(parameter,&__agentParamter);
    free_agent_para(parameter);

    parameter = create_agent_para_factory("/nowhere");
    EXPECT_EQ(parameter,&__agentParamter);
    free_agent_para(parameter);

    parameter = create_agent_para_factory("./config/pinpoint_agent_test.config");
    EXPECT_STREQ(parameter->commonConf.agent_type,"TEST");
    EXPECT_STREQ(parameter->commonConf.agentId,"111111");
    EXPECT_STREQ(parameter->commonConf.collector_tcp_ip,"127.0.0.1");
    EXPECT_STREQ(parameter->commonConf.applicationName,"test_pinpoint");
    EXPECT_EQ(parameter->commonConf.collector_span_port, (uint32_t)29993);
    free_agent_para(parameter);
}

TEST(config,PHP_plugin_init_and_free)
{
	init_pinpoint_agent(NULL);
	free_pinpoint_agent();

	init_pinpoint_agent("/point_to_mars");
	free_pinpoint_agent();

	init_pinpoint_agent("./config/pinpoint_agent_test.config");
	free_pinpoint_agent();

}

#endif


