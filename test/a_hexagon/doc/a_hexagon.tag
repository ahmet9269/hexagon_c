<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>mainpage.md</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/doc/</path>
    <filename>dc/d02/mainpage_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>AdapterManager.hpp</name>
    <path>a_hexagon/adapters/common/</path>
    <filename>d5/d00/AdapterManager_8hpp.html</filename>
    <includes id="d4/d01/MessagePipeline_8hpp" name="MessagePipeline.hpp" local="yes" import="no" module="no" objc="no">MessagePipeline.hpp</includes>
    <class kind="class">adapters::AdapterManager</class>
    <namespace>adapters</namespace>
  </compound>
  <compound kind="file">
    <name>IAdapter.hpp</name>
    <path>a_hexagon/adapters/common/</path>
    <filename>d0/d00/IAdapter_8hpp.html</filename>
    <class kind="class">adapters::IAdapter</class>
    <namespace>adapters</namespace>
  </compound>
  <compound kind="file">
    <name>MessagePipeline.hpp</name>
    <path>a_hexagon/adapters/common/</path>
    <filename>d4/d01/MessagePipeline_8hpp.html</filename>
    <includes id="d0/d00/IAdapter_8hpp" name="IAdapter.hpp" local="yes" import="no" module="no" objc="no">IAdapter.hpp</includes>
    <class kind="class">adapters::MessagePipeline</class>
    <namespace>adapters</namespace>
  </compound>
  <compound kind="file">
    <name>IMessageSocket.hpp</name>
    <path>a_hexagon/adapters/common/messaging/</path>
    <filename>d1/d03/IMessageSocket_8hpp.html</filename>
    <class kind="class">adapters::common::messaging::IMessageSocket</class>
    <namespace>adapters</namespace>
    <namespace>adapters::common</namespace>
    <namespace>adapters::common::messaging</namespace>
  </compound>
  <compound kind="file">
    <name>ZeroMQSocket.cpp</name>
    <path>a_hexagon/adapters/common/messaging/</path>
    <filename>d6/d02/ZeroMQSocket_8cpp.html</filename>
    <includes id="d4/d03/ZeroMQSocket_8hpp" name="ZeroMQSocket.hpp" local="yes" import="no" module="no" objc="no">ZeroMQSocket.hpp</includes>
    <includes id="d9/d01/Logger_8hpp" name="Logger.hpp" local="yes" import="no" module="no" objc="no">utils/Logger.hpp</includes>
    <namespace>adapters</namespace>
    <namespace>adapters::common</namespace>
    <namespace>adapters::common::messaging</namespace>
    <namespace>adapters::common::messaging::anonymous_namespace{ZeroMQSocket.cpp}</namespace>
  </compound>
  <compound kind="file">
    <name>ZeroMQSocket.hpp</name>
    <path>a_hexagon/adapters/common/messaging/</path>
    <filename>d4/d03/ZeroMQSocket_8hpp.html</filename>
    <includes id="d1/d03/IMessageSocket_8hpp" name="IMessageSocket.hpp" local="yes" import="no" module="no" objc="no">IMessageSocket.hpp</includes>
    <class kind="class">adapters::common::messaging::ZeroMQSocket</class>
    <namespace>adapters</namespace>
    <namespace>adapters::common</namespace>
    <namespace>adapters::common::messaging</namespace>
    <member kind="define">
      <type>#define</type>
      <name>ZMQ_DISH</name>
      <anchorfile>d4/d03/ZeroMQSocket_8hpp.html</anchorfile>
      <anchor>ad7847aff3e15f1fa8862ffad9385a827</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ZMQ_RADIO</name>
      <anchorfile>d4/d03/ZeroMQSocket_8hpp.html</anchorfile>
      <anchor>aa6eb91d89c3dc2108352335def5f322f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>TrackDataZeroMQIncomingAdapter.cpp</name>
    <path>a_hexagon/adapters/incoming/zeromq/</path>
    <filename>d5/d00/TrackDataZeroMQIncomingAdapter_8cpp.html</filename>
    <includes id="da/d03/TrackDataZeroMQIncomingAdapter_8hpp" name="TrackDataZeroMQIncomingAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp</includes>
    <includes id="d4/d03/ZeroMQSocket_8hpp" name="ZeroMQSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/ZeroMQSocket.hpp</includes>
    <includes id="d9/d01/Logger_8hpp" name="Logger.hpp" local="yes" import="no" module="no" objc="no">utils/Logger.hpp</includes>
    <namespace>adapters</namespace>
    <namespace>adapters::incoming</namespace>
    <namespace>adapters::incoming::zeromq</namespace>
  </compound>
  <compound kind="file">
    <name>TrackDataZeroMQIncomingAdapter.hpp</name>
    <path>a_hexagon/adapters/incoming/zeromq/</path>
    <filename>da/d03/TrackDataZeroMQIncomingAdapter_8hpp.html</filename>
    <includes id="d0/d00/IAdapter_8hpp" name="IAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/common/IAdapter.hpp</includes>
    <includes id="d1/d03/IMessageSocket_8hpp" name="IMessageSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/IMessageSocket.hpp</includes>
    <includes id="d1/d03/ITrackDataIncomingPort_8hpp" name="ITrackDataIncomingPort.hpp" local="yes" import="no" module="no" objc="no">domain/ports/incoming/ITrackDataIncomingPort.hpp</includes>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <class kind="class">adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter</class>
    <namespace>adapters</namespace>
    <namespace>adapters::incoming</namespace>
    <namespace>adapters::incoming::zeromq</namespace>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackDataZeroMQOutgoingAdapter.cpp</name>
    <path>a_hexagon/adapters/outgoing/zeromq/</path>
    <filename>d4/d00/ExtrapTrackDataZeroMQOutgoingAdapter_8cpp.html</filename>
    <includes id="d1/d01/ExtrapTrackDataZeroMQOutgoingAdapter_8hpp" name="ExtrapTrackDataZeroMQOutgoingAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp</includes>
    <includes id="d4/d03/ZeroMQSocket_8hpp" name="ZeroMQSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/ZeroMQSocket.hpp</includes>
    <includes id="d9/d01/Logger_8hpp" name="Logger.hpp" local="yes" import="no" module="no" objc="no">utils/Logger.hpp</includes>
    <namespace>adapters</namespace>
    <namespace>adapters::outgoing</namespace>
    <namespace>adapters::outgoing::zeromq</namespace>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackDataZeroMQOutgoingAdapter.hpp</name>
    <path>a_hexagon/adapters/outgoing/zeromq/</path>
    <filename>d1/d01/ExtrapTrackDataZeroMQOutgoingAdapter_8hpp.html</filename>
    <includes id="d0/d00/IAdapter_8hpp" name="IAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/common/IAdapter.hpp</includes>
    <includes id="d1/d03/IMessageSocket_8hpp" name="IMessageSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/IMessageSocket.hpp</includes>
    <includes id="d7/d01/IExtrapTrackDataOutgoingPort_8hpp" name="IExtrapTrackDataOutgoingPort.hpp" local="yes" import="no" module="no" objc="no">domain/ports/outgoing/IExtrapTrackDataOutgoingPort.hpp</includes>
    <includes id="d7/d00/model_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/model/ExtrapTrackData.hpp</includes>
    <class kind="class">adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter</class>
    <namespace>adapters</namespace>
    <namespace>adapters::outgoing</namespace>
    <namespace>adapters::outgoing::zeromq</namespace>
    <namespace>domain</namespace>
    <namespace>domain::adapters</namespace>
    <namespace>domain::adapters::outgoing</namespace>
  </compound>
  <compound kind="file">
    <name>TrackDataExtrapolator.cpp</name>
    <path>a_hexagon/domain/logic/</path>
    <filename>dc/d02/TrackDataExtrapolator_8cpp.html</filename>
    <includes id="d7/d03/TrackDataExtrapolator_8hpp" name="TrackDataExtrapolator.hpp" local="yes" import="no" module="no" objc="no">domain/logic/TrackDataExtrapolator.hpp</includes>
    <namespace>domain</namespace>
    <namespace>domain::logic</namespace>
  </compound>
  <compound kind="file">
    <name>TrackDataExtrapolator.hpp</name>
    <path>a_hexagon/domain/logic/</path>
    <filename>d7/d03/TrackDataExtrapolator_8hpp.html</filename>
    <includes id="d7/d01/IExtrapTrackDataOutgoingPort_8hpp" name="IExtrapTrackDataOutgoingPort.hpp" local="no" import="no" module="no" objc="no">domain/ports/outgoing/IExtrapTrackDataOutgoingPort.hpp</includes>
    <includes id="d1/d03/ITrackDataIncomingPort_8hpp" name="ITrackDataIncomingPort.hpp" local="yes" import="no" module="no" objc="no">domain/ports/incoming/ITrackDataIncomingPort.hpp</includes>
    <includes id="d9/d03/model_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/model/TrackData.hpp</includes>
    <includes id="d7/d00/model_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/model/ExtrapTrackData.hpp</includes>
    <class kind="class">domain::logic::TrackDataExtrapolator</class>
    <namespace>domain</namespace>
    <namespace>domain::logic</namespace>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackData.cpp</name>
    <path>a_hexagon/domain/model/</path>
    <filename>d3/d00/ExtrapTrackData_8cpp.html</filename>
    <includes id="d7/d00/model_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/model/ExtrapTrackData.hpp</includes>
    <namespace>domain</namespace>
    <namespace>domain::model</namespace>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackData.hpp</name>
    <path>a_hexagon/domain/model/</path>
    <filename>d7/d00/model_2ExtrapTrackData_8hpp.html</filename>
    <includes id="d3/d02/ports_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/ExtrapTrackData.hpp</includes>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackData.hpp</name>
    <path>a_hexagon/domain/ports/</path>
    <filename>d3/d02/ports_2ExtrapTrackData_8hpp.html</filename>
    <class kind="class">domain::model::ExtrapTrackData</class>
    <namespace>domain</namespace>
    <namespace>domain::model</namespace>
  </compound>
  <compound kind="file">
    <name>TrackData.cpp</name>
    <path>a_hexagon/domain/model/</path>
    <filename>df/d00/TrackData_8cpp.html</filename>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <namespace>domain</namespace>
    <namespace>domain::model</namespace>
  </compound>
  <compound kind="file">
    <name>TrackData.hpp</name>
    <path>a_hexagon/domain/model/</path>
    <filename>d9/d03/model_2TrackData_8hpp.html</filename>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
  </compound>
  <compound kind="file">
    <name>TrackData.hpp</name>
    <path>a_hexagon/domain/ports/</path>
    <filename>df/d03/ports_2TrackData_8hpp.html</filename>
    <class kind="class">domain::model::TrackData</class>
    <namespace>domain</namespace>
    <namespace>domain::model</namespace>
  </compound>
  <compound kind="file">
    <name>ITrackDataIncomingPort.hpp</name>
    <path>a_hexagon/domain/ports/incoming/</path>
    <filename>d1/d03/ITrackDataIncomingPort_8hpp.html</filename>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <class kind="class">domain::ports::incoming::ITrackDataIncomingPort</class>
    <namespace>domain</namespace>
    <namespace>domain::ports</namespace>
    <namespace>domain::ports::incoming</namespace>
  </compound>
  <compound kind="file">
    <name>IExtrapTrackDataOutgoingPort.hpp</name>
    <path>a_hexagon/domain/ports/outgoing/</path>
    <filename>d7/d01/IExtrapTrackDataOutgoingPort_8hpp.html</filename>
    <includes id="d7/d00/model_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/model/ExtrapTrackData.hpp</includes>
    <class kind="class">domain::ports::outgoing::IExtrapTrackDataOutgoingPort</class>
    <namespace>domain</namespace>
    <namespace>domain::ports</namespace>
    <namespace>domain::ports::outgoing</namespace>
  </compound>
  <compound kind="file">
    <name>main.cpp</name>
    <path>a_hexagon/</path>
    <filename>df/d02/main_8cpp.html</filename>
    <includes id="d9/d01/Logger_8hpp" name="Logger.hpp" local="yes" import="no" module="no" objc="no">utils/Logger.hpp</includes>
    <includes id="d5/d00/AdapterManager_8hpp" name="AdapterManager.hpp" local="yes" import="no" module="no" objc="no">adapters/common/AdapterManager.hpp</includes>
    <includes id="d4/d01/MessagePipeline_8hpp" name="MessagePipeline.hpp" local="yes" import="no" module="no" objc="no">adapters/common/MessagePipeline.hpp</includes>
    <includes id="d1/d03/IMessageSocket_8hpp" name="IMessageSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/IMessageSocket.hpp</includes>
    <includes id="d4/d03/ZeroMQSocket_8hpp" name="ZeroMQSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/ZeroMQSocket.hpp</includes>
    <includes id="da/d03/TrackDataZeroMQIncomingAdapter_8hpp" name="TrackDataZeroMQIncomingAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp</includes>
    <includes id="d1/d01/ExtrapTrackDataZeroMQOutgoingAdapter_8hpp" name="ExtrapTrackDataZeroMQOutgoingAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp</includes>
    <includes id="d7/d03/TrackDataExtrapolator_8hpp" name="TrackDataExtrapolator.hpp" local="yes" import="no" module="no" objc="no">domain/logic/TrackDataExtrapolator.hpp</includes>
    <namespace>config</namespace>
    <member kind="function">
      <type>void</type>
      <name>signalHandler</name>
      <anchorfile>df/d02/main_8cpp.html</anchorfile>
      <anchor>a8ee3282bc313e547dbbb8d4f4010db61</anchor>
      <arglist>(int signum)</arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; adapters::common::messaging::IMessageSocket &gt;</type>
      <name>createIncomingSocket</name>
      <anchorfile>df/d02/main_8cpp.html</anchorfile>
      <anchor>ada6cf0fc84b2c5041e61d433ec3804d0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; adapters::common::messaging::IMessageSocket &gt;</type>
      <name>createOutgoingSocket</name>
      <anchorfile>df/d02/main_8cpp.html</anchorfile>
      <anchor>acce2b6a123257effa76401b012f6efd2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>df/d02/main_8cpp.html</anchorfile>
      <anchor>ae66f6b31b5ad750f1fe042a706a4e3d4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::atomic&lt; bool &gt;</type>
      <name>g_running</name>
      <anchorfile>df/d02/main_8cpp.html</anchorfile>
      <anchor>ac1b70aca9df5fb9f04b122b30ef4006e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>main.hpp</name>
    <path>a_hexagon/</path>
    <filename>d0/d00/main_8hpp.html</filename>
    <member kind="function">
      <type>void</type>
      <name>signalHandler</name>
      <anchorfile>d0/d00/main_8hpp.html</anchorfile>
      <anchor>a8ee3282bc313e547dbbb8d4f4010db61</anchor>
      <arglist>(int signum)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>initializeLogging</name>
      <anchorfile>d0/d00/main_8hpp.html</anchorfile>
      <anchor>aa82b7aef5785a968333d6e26b9dc6c7c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setupSignalHandlers</name>
      <anchorfile>d0/d00/main_8hpp.html</anchorfile>
      <anchor>aa1fdb229b4728961ef997fae3e98b508</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::atomic&lt; bool &gt;</type>
      <name>g_shutdown_requested</name>
      <anchorfile>d0/d00/main_8hpp.html</anchorfile>
      <anchor>ad9223bbbed1d71323a789daf88911c4a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Logger.hpp</name>
    <path>a_hexagon/utils/</path>
    <filename>d9/d01/Logger_8hpp.html</filename>
    <class kind="class">utils::Logger</class>
    <class kind="class">Logger</class>
    <namespace>utils</namespace>
    <member kind="define">
      <type>#define</type>
      <name>LOG_TRACE</name>
      <anchorfile>d9/d01/Logger_8hpp.html</anchorfile>
      <anchor>a58c994180e09d2f6400133b19b1d7036</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LOG_DEBUG</name>
      <anchorfile>d9/d01/Logger_8hpp.html</anchorfile>
      <anchor>a754b3d074e0af4ad3c7b918dd77ecb2d</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LOG_INFO</name>
      <anchorfile>d9/d01/Logger_8hpp.html</anchorfile>
      <anchor>a378e28bfcb78d17285210d6bbb70a083</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LOG_WARN</name>
      <anchorfile>d9/d01/Logger_8hpp.html</anchorfile>
      <anchor>add82efa459e0af380b68522c29b9fd44</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LOG_ERROR</name>
      <anchorfile>d9/d01/Logger_8hpp.html</anchorfile>
      <anchor>ad4a9117ce894e3319e903142347a0f63</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LOG_CRITICAL</name>
      <anchorfile>d9/d01/Logger_8hpp.html</anchorfile>
      <anchor>a4091a2d7d849a8b5eded40d8723b8768</anchor>
      <arglist>(...)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>AdapterManagerTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/adapters/common/</path>
    <filename>d1/d03/AdapterManagerTest_8cpp.html</filename>
    <includes id="d5/d00/AdapterManager_8hpp" name="AdapterManager.hpp" local="yes" import="no" module="no" objc="no">adapters/common/AdapterManager.hpp</includes>
    <includes id="d0/d00/IAdapter_8hpp" name="IAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/common/IAdapter.hpp</includes>
    <includes id="d4/d01/MessagePipeline_8hpp" name="MessagePipeline.hpp" local="yes" import="no" module="no" objc="no">adapters/common/MessagePipeline.hpp</includes>
    <class kind="class">MockAdapter</class>
    <class kind="class">AdapterManagerTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>ad9fd6d9411ff394218ad697007afba04</anchor>
      <arglist>(AdapterManagerTest, DefaultConstructor_NotRunning)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a3cc170997070a73448cbb5beab5b4c67</anchor>
      <arglist>(AdapterManagerTest, DefaultConstructor_EmptyPipelines)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a94721c1b0cb90975aec106cbb2001bab</anchor>
      <arglist>(AdapterManagerTest, RegisterPipeline_SinglePipeline_Success)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a21f627a1caede2a700e6d08eed44eba6</anchor>
      <arglist>(AdapterManagerTest, RegisterPipeline_MultiplePipelines_AllRegistered)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a1470e9e50db7fd5db2008483236e5015</anchor>
      <arglist>(AdapterManagerTest, StartAll_CallsStartOnAllPipelines)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a7ad806f8e2974527e2e8008da3cbf494</anchor>
      <arglist>(AdapterManagerTest, StartAll_OneAdapterFails_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>ae0fd2f935b993a898c4f99bb0be359c4</anchor>
      <arglist>(AdapterManagerTest, StopAll_CallsStopOnAllAdapters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a6f25781ad5056914e121c0179fbc3985</anchor>
      <arglist>(AdapterManagerTest, StopAll_WhenNotStarted_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a0eafc1a01c6d7e4fe376f097a7236ed7</anchor>
      <arglist>(AdapterManagerTest, IsRunning_AfterStart_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>ab0b2e7b41d629db507a007fa171d17f6</anchor>
      <arglist>(AdapterManagerTest, IsRunning_AfterStop_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a46348ef75844a1ce298a1816a5ffda66</anchor>
      <arglist>(AdapterManagerTest, GetPipeline_ByName_ReturnsCorrectPipeline)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a0177e30831c22462108369a56302ddb7</anchor>
      <arglist>(AdapterManagerTest, GetPipeline_NonExistent_ReturnsNull)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a9d41e9fbbcddb80584027788809baf0e</anchor>
      <arglist>(AdapterManagerTest, StartAll_NoPipelines_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a1bd436dfca6ef6d0022cc3b8758a6760</anchor>
      <arglist>(AdapterManagerTest, StopAll_NoPipelines_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>a0b32ea3984345583b6e256dcbc4b3779</anchor>
      <arglist>(AdapterManagerTest, GetPipelineCount_EmptyManager_ReturnsZero)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d1/d03/AdapterManagerTest_8cpp.html</anchorfile>
      <anchor>adcd76508ff0c4dfdfe31651613ac38e4</anchor>
      <arglist>(AdapterManagerTest, GetPipelineCount_AfterRegister_ReturnsCorrectCount)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>TrackDataZeroMQIncomingAdapterTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/adapters/incoming/</path>
    <filename>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</filename>
    <includes id="da/d03/TrackDataZeroMQIncomingAdapter_8hpp" name="TrackDataZeroMQIncomingAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp</includes>
    <includes id="db/d03/MockMessageSocket_8hpp" name="MockMessageSocket.hpp" local="yes" import="no" module="no" objc="no">../../mocks/MockMessageSocket.hpp</includes>
    <includes id="d8/d00/MockIncomingPort_8hpp" name="MockIncomingPort.hpp" local="yes" import="no" module="no" objc="no">../../mocks/MockIncomingPort.hpp</includes>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <class kind="class">TrackDataZeroMQIncomingAdapterTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>aa86f9bef69a5802c0009a9cfe42053e8</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Constructor_WithValidDependencies_CreatesAdapter)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>aa4c6240337f4f1702198fe92ec46dc9a</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Start_WithConnectedSocket_StartsSuccessfully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a08661ff6cd06b5d94d5bc805867146ea</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Start_WhenAlreadyRunning_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>ae4b31434ef86770aef1ea4055daa00a3</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Stop_WhenRunning_StopsAdapter)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a899c7bb7a06ea6b727d02eea1b4a796e</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Stop_WhenNotRunning_DoesNothing)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a3f20d8f2a816d047d35ce620efde8c3c</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Destructor_WhenRunning_StopsGracefully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>afb00c59f086ab130a2d3b44f8177fab4</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_WithValidMessage_ForwardsToPort)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a9bea26ff90dd3899e7b12db4ed910e4e</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_WithMultipleMessages_ForwardsAllToPort)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>adbb46186c06c1ec455b7076ff8a143a5</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_WithInvalidMessage_SkipsAndContinues)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a0b7e693535b03bfc2db9224c5d62f51c</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_WithEmptyMessage_HandlesGracefully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>ad97d5b44f9e7788fc37b218726ee338a</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, Start_WithFailedConnect_HandlesError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a9e03f70bc58737723eff8b39005bb617</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_WhenSocketDisconnects_StopsGracefully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a75289ccfb5c303877384aa562bc0faa3</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_PreservesTrackId)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a0b731deefd314167a7dd5c82e93704c3</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_PreservesPosition)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a56f5740d13aecf5ad7681295eea80c65</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_PreservesVelocity)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>ab897da00308cd13ae98eb54468ac99b8</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, HighThroughput_ProcessesMessagesQuickly)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>aaf7c6dc9ea385f3049b4ba1ff02bee38</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ConcurrentStartStop_IsThreadSafe)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>ac348f8492c366a0851705e871fd6ac8a</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_VerifySocketReceiveCalls)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d6/d03/TrackDataZeroMQIncomingAdapterTest_8cpp.html</anchorfile>
      <anchor>a7659b437734601e014d925bd9b2acfca</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapterTest, ReceiveLoop_VerifyPortProcessCalls)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackDataZeroMQOutgoingAdapterTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/adapters/outgoing/</path>
    <filename>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</filename>
    <includes id="d1/d01/ExtrapTrackDataZeroMQOutgoingAdapter_8hpp" name="ExtrapTrackDataZeroMQOutgoingAdapter.hpp" local="yes" import="no" module="no" objc="no">adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp</includes>
    <includes id="db/d03/MockMessageSocket_8hpp" name="MockMessageSocket.hpp" local="yes" import="no" module="no" objc="no">../../mocks/MockMessageSocket.hpp</includes>
    <includes id="d3/d02/ports_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/ExtrapTrackData.hpp</includes>
    <class kind="class">ExtrapTrackDataZeroMQOutgoingAdapterTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a5c0deb840eabcfb1b8cdb3851ce127d3</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, Constructor_WithValidSocket_CreatesAdapter)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a300459f049b5efd5ca58039241001ad2</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, Start_WithConnectedSocket_StartsSuccessfully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a04f4b7dc51035ee89c0aa07deaebcaa2</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, Start_WhenAlreadyRunning_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>af66fc14c2b761f07898b566783dcf8e9</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, Stop_WhenRunning_StopsAdapter)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a09a47abadb189c63d6d9e0e18f9ab44a</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, Stop_WhenNotRunning_DoesNothing)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a8ef8f1148230e58abca2b8cec5844b1b</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, Destructor_WhenRunning_StopsGracefully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>aa93c017a85b69f800850f98f950ab789</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_SingleMessage_SendsSuccessfully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>ad754533ba35a58caf51a51b23219dbd8</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_Vector_SendsAllMessages)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a52362be79b99c3b0c102b192400f3ac7</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_EmptyVector_SendsNothing)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a6f320a31e6d098c632073a918aa58564</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_WhenNotRunning_DoesNotSend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a63f64945a9cb31814cf4461dee162f4a</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_AfterStop_DoesNotSend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a08f997807fc25c79b224013fc1cb8759</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_PreservesTrackId)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>aaa0e7f79a37ea6490302188d8e7e1fb0</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_PreservesPosition)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a0dc51f2e92d71bbdaeabf479df7bf04b</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_PreservesVelocity)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a4cec407e1fd4cc71fd6415c2129f762b</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_PreservesTimestamps)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a54286ee005d0ea7a5cab1e8f6b3a196b</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_WhenSocketFails_HandlesGracefully)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a5518698a77743b6a05ddf3cd2309298b</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, SendExtrapTrackData_SetsGroupName)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a3c21a89d85400a5d67867e403d378060</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, HighThroughput_SendsMessagesQuickly)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>a7157765e7228fda229923150af5554b8</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, ConcurrentSends_AreThreadSafe)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d01/ExtrapTrackDataZeroMQOutgoingAdapterTest_8cpp.html</anchorfile>
      <anchor>aaf38f537d84a3c2b19e4b345c90cfa89</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapterTest, ConcurrentStartStop_IsThreadSafe)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>TrackDataExtrapolatorTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/domain/logic/</path>
    <filename>d3/d00/TrackDataExtrapolatorTest_8cpp.html</filename>
    <includes id="d7/d03/TrackDataExtrapolator_8hpp" name="TrackDataExtrapolator.hpp" local="yes" import="no" module="no" objc="no">domain/logic/TrackDataExtrapolator.hpp</includes>
    <includes id="d1/d00/MockOutgoingPort_8hpp" name="MockOutgoingPort.hpp" local="yes" import="no" module="no" objc="no">../../mocks/MockOutgoingPort.hpp</includes>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <includes id="d3/d02/ports_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/ExtrapTrackData.hpp</includes>
    <class kind="class">TrackDataExtrapolatorTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a795a1c1cd7dc4bf67e681d9063fbddbb</anchor>
      <arglist>(TrackDataExtrapolatorTest, Constructor_WithValidPort_CreatesExtrapolator)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>aea69c88a68b45388458a9f84681718b4</anchor>
      <arglist>(TrackDataExtrapolatorTest, Constructor_WithNullPort_DoesNotCrash)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a20ace6c1ebfcf0fbe93e9acc38f6c77b</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_GeneratesMultipleOutputs)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>aa42b48b29bb1fe56f55a802882c4bafd</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_PreservesTrackId)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a8d1a4129956c560527b4c5a55292f7c4</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_PreservesVelocity)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a7747782ada7b8edc5fd1147fb8462aab</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_PreservesOriginalUpdateTime)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>aed2f049f8f5f58fc7f02bd6198c45863</anchor>
      <arglist>(TrackDataExtrapolatorTest, ExtrapolateTrackData_CalculatesPositionCorrectly)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a797a0811e37858a67da17a9b019240cf</anchor>
      <arglist>(TrackDataExtrapolatorTest, ExtrapolateTrackData_IncrementsUpdateTime)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a0a3f76200444b80c25236f603590c3b5</anchor>
      <arglist>(TrackDataExtrapolatorTest, ExtrapolateTrackData_SetsFirstHopSentTime)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a2bef81623c770275d18bace48747d023</anchor>
      <arglist>(TrackDataExtrapolatorTest, FrequencyInterval_8HzInput_125msInterval)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>ad346f492467cfb14b66e4eb3bbfed56d</anchor>
      <arglist>(TrackDataExtrapolatorTest, FrequencyInterval_100HzOutput_10msInterval)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a7090d32804548ea1026cd84bd4eb22f1</anchor>
      <arglist>(TrackDataExtrapolatorTest, ExtrapolationCount_8HzTo100Hz_Approximately12Outputs)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>abdcecce3933aa17b0b7eeefad3b760d3</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WithZeroVelocity_PositionUnchanged)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>adaab8c43f2634e5c3b6147e7d0566e2a</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WithNegativeVelocity_DecrementsPosition)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>ad7d11cab535c5599b07845ee94e9aff2</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WithLargeVelocity_HandlesCorrectly)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a80a8e79b6b4d77d814223ef359378516</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_CallsSendForEachOutput)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a36eea4e579fd1967209f212844df80d0</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_MultipleInputs_ProcessesAll)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d3/d00/TrackDataExtrapolatorTest_8cpp.html</anchorfile>
      <anchor>a574c1bc0fb9c06483d95b373f104924c</anchor>
      <arglist>(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WhenPortFails_HandlesGracefully)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ExtrapTrackDataTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/domain/model/</path>
    <filename>de/d00/ExtrapTrackDataTest_8cpp.html</filename>
    <includes id="d3/d02/ports_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/ExtrapTrackData.hpp</includes>
    <class kind="class">ExtrapTrackDataTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>ab1d060a5bb32722749bc0e157493f6a2</anchor>
      <arglist>(ExtrapTrackDataTest, DefaultConstructor_CreatesValidObject)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a268dd0822615e1807346cb74033ec1de</anchor>
      <arglist>(ExtrapTrackDataTest, CopyConstructor_CopiesAllFields)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>af3fe991dd6d5e58a1068c58a8dd14fbf</anchor>
      <arglist>(ExtrapTrackDataTest, MoveConstructor_MovesAllFields)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a1affaec9ba776556e3368d343b2df89d</anchor>
      <arglist>(ExtrapTrackDataTest, SetUpdateTime_GetUpdateTime_ReturnsCorrectValue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a3c4412599357747bad2bfb498dafdee2</anchor>
      <arglist>(ExtrapTrackDataTest, SetFirstHopSentTime_GetFirstHopSentTime_ReturnsCorrectValue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a848a15f82b5882962e07f41219c4b3c2</anchor>
      <arglist>(ExtrapTrackDataTest, AllTimingFields_CanBeSetAndRetrieved)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>ade11afcb92bada8d42da3175f59631de</anchor>
      <arglist>(ExtrapTrackDataTest, SetPosition_ECEF_CoordinatesSet)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>adfee8d533cef48646df91e370f08774b</anchor>
      <arglist>(ExtrapTrackDataTest, SetVelocity_ECEF_VelocitiesSet)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a0a4c28dfe41b760331f53fb7d4c69da0</anchor>
      <arglist>(ExtrapTrackDataTest, IsValid_ValidData_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a1379fd5c361ccd53b316d97707c13389</anchor>
      <arglist>(ExtrapTrackDataTest, IsValid_DefaultConstructed_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a7a287c386bc8145a35fea2e1a0317b57</anchor>
      <arglist>(ExtrapTrackDataTest, Serialize_ReturnsNonEmptyVector)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a220640cde493714843117f6e9c2e2432</anchor>
      <arglist>(ExtrapTrackDataTest, GetSerializedSize_ReturnsExpectedSize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a6820e5e180058495e89cdd3394825bd6</anchor>
      <arglist>(ExtrapTrackDataTest, SerializeDeserialize_RoundTrip_DataPreserved)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a795be4cba7aa79464ec6d0d0f2ccce20</anchor>
      <arglist>(ExtrapTrackDataTest, Deserialize_InvalidSize_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>ac9d3082de67f7d8f541cb1f999b3bbee</anchor>
      <arglist>(ExtrapTrackDataTest, Deserialize_EmptyVector_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>ab2279eb7dff12ef3ed6bb383895b9583</anchor>
      <arglist>(ExtrapTrackDataTest, TimingFields_MaxInt64_ThrowsException)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a0adc6bee432f2cf306e674e4fbb2ba5b</anchor>
      <arglist>(ExtrapTrackDataTest, TimingConsistency_UpdateTimeAfterOriginal)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>de/d00/ExtrapTrackDataTest_8cpp.html</anchorfile>
      <anchor>a3c997b45cebdc0cd7f6b9dcfec4f7bfb</anchor>
      <arglist>(ExtrapTrackDataTest, TimingConsistency_FirstHopBetweenOriginalAndUpdate)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>TrackDataTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/domain/model/</path>
    <filename>dd/d03/TrackDataTest_8cpp.html</filename>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <class kind="class">TrackDataTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a39ec1f20dd14318f822be894472c65fb</anchor>
      <arglist>(TrackDataTest, DefaultConstructor_CreatesValidObject)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>ad5342fc235f508d86ce2e286d0ad9f89</anchor>
      <arglist>(TrackDataTest, CopyConstructor_CopiesAllFields)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>aac99fa46263d96a325f66c3d29c7690a</anchor>
      <arglist>(TrackDataTest, MoveConstructor_MovesAllFields)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>af85c4e64615147efb6add88ab32766b7</anchor>
      <arglist>(TrackDataTest, SetTrackId_GetTrackId_ReturnsCorrectValue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a3aef55873bf34a976d0390e707d7ca86</anchor>
      <arglist>(TrackDataTest, SetTrackId_NegativeValue_ThrowsException)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>aa752fac4a6b32f31e0cfdec15c7bae3b</anchor>
      <arglist>(TrackDataTest, SetPosition_ECEF_CoordinatesSet)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a610f9a8e3614644ea2b4846b212f303a</anchor>
      <arglist>(TrackDataTest, SetVelocity_ECEF_VelocitiesSet)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>ab44e84242496d91673f80188c3433bd6</anchor>
      <arglist>(TrackDataTest, SetOriginalUpdateTime_ValidTimestamp_Set)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a0fb4b4cadf758b4fed724ac68276bf93</anchor>
      <arglist>(TrackDataTest, IsValid_ValidData_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>aeae422cd40224fcbd9f85b7c619b3390</anchor>
      <arglist>(TrackDataTest, IsValid_DefaultConstructed_ChecksValidity)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a5ca1d7545b892637341d58ffe9f67af1</anchor>
      <arglist>(TrackDataTest, Serialize_ReturnsNonEmptyVector)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>ad9bcb250f58757f84fdb1ce50b45ccc2</anchor>
      <arglist>(TrackDataTest, GetSerializedSize_ReturnsExpectedSize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>ac3c262f5e09f1bac9b2cd15fe9f900f2</anchor>
      <arglist>(TrackDataTest, SerializeDeserialize_RoundTrip_DataPreserved)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>af6df184fba5e3467356d1f10dedf39c1</anchor>
      <arglist>(TrackDataTest, Deserialize_InvalidSize_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a682f474cb988038c8b83895879d36903</anchor>
      <arglist>(TrackDataTest, Deserialize_EmptyVector_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>ac524a2edcd875f8731cbc1c380ac4f74</anchor>
      <arglist>(TrackDataTest, SetTrackId_MaxInt32_Succeeds)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a8e838d1a1830df5e61c2f3c9ad45a5e5</anchor>
      <arglist>(TrackDataTest, SetTrackId_MinInt32_ThrowsException)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>abcda439274d3197bf08680b096b7914b</anchor>
      <arglist>(TrackDataTest, SetPosition_LargeValues_Succeeds)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a9860e59e8f4febf1b654f17f618305b2</anchor>
      <arglist>(TrackDataTest, CopyAssignment_CopiesAllFields)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>dd/d03/TrackDataTest_8cpp.html</anchorfile>
      <anchor>a12224e530b1a1ac5621060210e5ecbfa</anchor>
      <arglist>(TrackDataTest, MoveAssignment_MovesAllFields)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>main_test.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/</path>
    <filename>d5/d01/main__test_8cpp.html</filename>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>d5/d01/main__test_8cpp.html</anchorfile>
      <anchor>a3c04138a5bfe5d72780bb7e82a18e627</anchor>
      <arglist>(int argc, char **argv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>MockIncomingPort.hpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/mocks/</path>
    <filename>d8/d00/MockIncomingPort_8hpp.html</filename>
    <includes id="d1/d03/ITrackDataIncomingPort_8hpp" name="ITrackDataIncomingPort.hpp" local="yes" import="no" module="no" objc="no">domain/ports/incoming/ITrackDataIncomingPort.hpp</includes>
    <includes id="df/d03/ports_2TrackData_8hpp" name="TrackData.hpp" local="yes" import="no" module="no" objc="no">domain/ports/TrackData.hpp</includes>
    <class kind="class">a_hexagon::test::mocks::MockIncomingPort</class>
    <namespace>a_hexagon</namespace>
    <namespace>a_hexagon::test</namespace>
    <namespace>a_hexagon::test::mocks</namespace>
  </compound>
  <compound kind="file">
    <name>MockMessageSocket.hpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/mocks/</path>
    <filename>db/d03/MockMessageSocket_8hpp.html</filename>
    <includes id="d1/d03/IMessageSocket_8hpp" name="IMessageSocket.hpp" local="yes" import="no" module="no" objc="no">adapters/common/messaging/IMessageSocket.hpp</includes>
    <class kind="class">a_hexagon::test::mocks::MockMessageSocket</class>
    <class kind="struct">a_hexagon::test::mocks::MockMessageSocket::SentMessage</class>
    <namespace>a_hexagon</namespace>
    <namespace>a_hexagon::test</namespace>
    <namespace>a_hexagon::test::mocks</namespace>
  </compound>
  <compound kind="file">
    <name>MockOutgoingPort.hpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/mocks/</path>
    <filename>d1/d00/MockOutgoingPort_8hpp.html</filename>
    <includes id="d7/d01/IExtrapTrackDataOutgoingPort_8hpp" name="IExtrapTrackDataOutgoingPort.hpp" local="yes" import="no" module="no" objc="no">domain/ports/outgoing/IExtrapTrackDataOutgoingPort.hpp</includes>
    <includes id="d7/d00/model_2ExtrapTrackData_8hpp" name="ExtrapTrackData.hpp" local="yes" import="no" module="no" objc="no">domain/model/ExtrapTrackData.hpp</includes>
    <class kind="class">a_hexagon::test::mocks::MockOutgoingPort</class>
    <class kind="struct">a_hexagon::test::mocks::MockOutgoingPort::SentRecord</class>
    <namespace>a_hexagon</namespace>
    <namespace>a_hexagon::test</namespace>
    <namespace>a_hexagon::test::mocks</namespace>
  </compound>
  <compound kind="file">
    <name>README.md</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/</path>
    <filename>da/d01/README_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>README.md</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/</path>
    <filename>d5/d01/test_2a__hexagon_2README_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>LoggerTest.cpp</name>
    <path>/workspaces/hexagon_c/test/a_hexagon/test/a_hexagon/utils/</path>
    <filename>d5/d02/LoggerTest_8cpp.html</filename>
    <includes id="d9/d01/Logger_8hpp" name="Logger.hpp" local="yes" import="no" module="no" objc="no">utils/Logger.hpp</includes>
    <class kind="class">LoggerTest</class>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>ad895ef8683ce6a8c255c138809a2cbf9</anchor>
      <arglist>(LoggerTest, Init_DefaultParameters_Success)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>ac02a34932a37e2bc5c71d929feea088d</anchor>
      <arglist>(LoggerTest, Init_CustomQueueSize_Success)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a2fedd4292451b22706bd297634671018</anchor>
      <arglist>(LoggerTest, Init_MultipleThreads_Success)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a4b3379375a5df6908052147b45316313</anchor>
      <arglist>(LoggerTest, InitWithFile_ValidPath_Success)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a625cd727e3a26f6ab2f0d6cc76383e5f</anchor>
      <arglist>(LoggerTest, DoubleInit_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a2e997047de3ccf471295c2aa181ab0d4</anchor>
      <arglist>(LoggerTest, Shutdown_AfterInit_Success)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a339003c8b0e52f89d480d7d6ec83834a</anchor>
      <arglist>(LoggerTest, Shutdown_BeforeInit_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>aa2bc9da6828fbe48da77e2229aa680e5</anchor>
      <arglist>(LoggerTest, DoubleShutdown_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>ac3ebf1c2751e8d36d6c7f5b30c6630d5</anchor>
      <arglist>(LoggerTest, SetLevel_AllLevels_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a614ec4d50d707fae91da7313234db58e</anchor>
      <arglist>(LoggerTest, Trace_ValidMessage_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>afe39a4470be5b33b274d36a5b32c8056</anchor>
      <arglist>(LoggerTest, Debug_ValidMessage_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a06d6b2496c8b9553486f3ac07d61415c</anchor>
      <arglist>(LoggerTest, Info_ValidMessage_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a2d40514d574f3b8cc6dbb34af8e1e582</anchor>
      <arglist>(LoggerTest, Warn_ValidMessage_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>abd63d55fd2152bc0b3b4ec60bb0137d5</anchor>
      <arglist>(LoggerTest, Error_ValidMessage_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>aad9b26a35ff8b05d4ba977b8b7af7225</anchor>
      <arglist>(LoggerTest, Critical_ValidMessage_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a572c0502f5eeef418862ef95487fd939</anchor>
      <arglist>(LoggerTest, Info_WithFormatArgs_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a7ccd9cf3ec676bc927c6ae0ec0b94fe0</anchor>
      <arglist>(LoggerTest, Debug_WithMultipleArgs_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a50fe7c02b6a5d4fe32108908ef94a8ea</anchor>
      <arglist>(LoggerTest, LogLatency_ValidParams_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a66f4ed7288ac7bfe9d2150d7bf5595a3</anchor>
      <arglist>(LoggerTest, LogLatency_ZeroLatency_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>afc0e27b9894e1faf6b4dcc8e4f508935</anchor>
      <arglist>(LoggerTest, LogLatency_LargeLatency_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>aac4fc9a7f31ee6ddb3700b2739dd3b87</anchor>
      <arglist>(LoggerTest, LogTrackReceived_ValidParams_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>ac81b6f2f25b05a6fb4507448ffaee7fa</anchor>
      <arglist>(LoggerTest, LogTrackReceived_NegativeId_NoThrow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>ab40d26a77b3ebc3fab165b78c8cddf44</anchor>
      <arglist>(LoggerTest, ConcurrentLogging_NoDataRace)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a038d76499203244350dc637f0c6ef720</anchor>
      <arglist>(LoggerTest, ConcurrentInitShutdown_NoDeadlock)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a4c5a1ca782d9c5d48ddb77ccef75cba5</anchor>
      <arglist>(LoggerTest, LoggingLatency_UnderThreshold)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a69b68a3a9f1b62c7ea1849c00cef179f</anchor>
      <arglist>(LoggerTest, LOG_INFO_Macro_Works)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>af5d336b23f773b1148c1bbf92d9780a2</anchor>
      <arglist>(LoggerTest, LOG_DEBUG_Macro_Works)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a0d0e31ee6e18b780528ae9ae6daec2fd</anchor>
      <arglist>(LoggerTest, LOG_ERROR_Macro_Works)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a3522b81c8c6e6596cb904bac8947b5e8</anchor>
      <arglist>(LoggerTest, IsInitialized_BeforeInit_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a2b6de0d6620bd6d847d19830bd89a620</anchor>
      <arglist>(LoggerTest, IsInitialized_AfterInit_ReturnsTrue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>ae7af49365148e04f8584b4d703fcd16e</anchor>
      <arglist>(LoggerTest, IsInitialized_AfterShutdown_ReturnsFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a7eca13705f8a68dfda8d8873c828ac9a</anchor>
      <arglist>(LoggerTest, EmptyAppName_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a0da16bd58f5e5c46942ada0b3da43c48</anchor>
      <arglist>(LoggerTest, VeryLongMessage_NoError)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_F</name>
      <anchorfile>d5/d02/LoggerTest_8cpp.html</anchorfile>
      <anchor>a3f98a91a8537cb41e0ed42b8dac073a8</anchor>
      <arglist>(LoggerTest, SpecialCharacters_NoError)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::AdapterManager</name>
    <filename>d1/d02/classadapters_1_1AdapterManager.html</filename>
    <member kind="function">
      <type></type>
      <name>AdapterManager</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a6611af26cd0c5c153339f97fc8769ef6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AdapterManager</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a223f36f9d52ff86467860de118e41fbe</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AdapterManager</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a81b4e92faf79f76a912787d8054f92b3</anchor>
      <arglist>(const AdapterManager &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>AdapterManager &amp;</type>
      <name>operator=</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a27f264f226eb35b970dd968a9750c66d</anchor>
      <arglist>(const AdapterManager &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AdapterManager</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>ac9c51f268ed77cd85030a71617c43fe6</anchor>
      <arglist>(AdapterManager &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>AdapterManager &amp;</type>
      <name>operator=</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a9823c0bdcfee88b7e51acd66ccf1cff2</anchor>
      <arglist>(AdapterManager &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>registerPipeline</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>ab0d33febc0c6ebf1f8214629aa667699</anchor>
      <arglist>(MessagePipeline pipeline)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>startAll</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a5c65f1bea8587352914aa1b34cd093b9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stopAll</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a733765e3a1e5b4c99f4f2162dbe96f95</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>getPipelineCount</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>ad63c2c791587e7a4b5924dfc2bba1266</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isRunning</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a063037ae77316ede7b2fc346ce2f3496</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>MessagePipeline *</type>
      <name>getPipeline</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a8198de0802a62d0b68cbada6a1b6e8e7</anchor>
      <arglist>(const std::string &amp;name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>const MessagePipeline *</type>
      <name>getPipeline</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a7d59c2fed71b0803c015bf5defc290ca</anchor>
      <arglist>(const std::string &amp;name) const noexcept</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::shared_mutex</type>
      <name>pipelines_mutex_</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a9950df3adbb11be2609e224ac7baef0a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; MessagePipeline &gt;</type>
      <name>pipelines_</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a041480d4461548ffc80ed66edbb120eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; bool &gt;</type>
      <name>running_</name>
      <anchorfile>d1/d02/classadapters_1_1AdapterManager.html</anchorfile>
      <anchor>a44cf3995f05d26a02cd8ba3df71bc7d2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AdapterManagerTest</name>
    <filename>d4/d01/classAdapterManagerTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>d4/d01/classAdapterManagerTest.html</anchorfile>
      <anchor>a1fd04a26ad2828cfa3d11e364af8d637</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>d4/d01/classAdapterManagerTest.html</anchorfile>
      <anchor>ab5dd57b3a70d815de5d58bf5c0b2e122</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; AdapterManager &gt;</type>
      <name>manager_</name>
      <anchorfile>d4/d01/classAdapterManagerTest.html</anchorfile>
      <anchor>a245bb5e086ff43e9b952e527cd7bbb37</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>domain::model::ExtrapTrackData</name>
    <filename>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</filename>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackData</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>adbf666dc41d6a0934d79bd0b9a84e0a7</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackData</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a95c221fba5b964da9c43a7c718778407</anchor>
      <arglist>(const ExtrapTrackData &amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackData</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a208d37e466542ea1d5230d00a85ec696</anchor>
      <arglist>(ExtrapTrackData &amp;&amp;other) noexcept=default</arglist>
    </member>
    <member kind="function">
      <type>ExtrapTrackData &amp;</type>
      <name>operator=</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a9d0aaaf2d723907d8be1ab3b828df80a</anchor>
      <arglist>(const ExtrapTrackData &amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type>ExtrapTrackData &amp;</type>
      <name>operator=</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>acc591e466e87c606ff02c9d73a44ed76</anchor>
      <arglist>(ExtrapTrackData &amp;&amp;other) noexcept=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ExtrapTrackData</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>acf9703da84330b2d5b613571537203a5</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>getTrackId</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a14a680e680316d696bb5bd3d64aa329c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTrackId</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ad32e1b75741b42645ac616abe10a8657</anchor>
      <arglist>(const int32_t &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getXVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ac5ede5b8655f0bb0398928ce89c04dee</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setXVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a3b82e8b1cdae8ae8507e7f08d7e6a949</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getYVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>afaffdaeab0a70cce706ecf489fa5c0f6</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setYVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>adf018f42d1e6fe5617c61e14d0e453bb</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a9b285aae934d17dfd3a8167fafa4d4c3</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>aba3877ce2c6ffc36f046fc1f4c16921d</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getXPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a2a7419c775b5f0a22228964fcf66e0c5</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setXPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a07d65c00cab19d6b4f56fa0314f2626b</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getYPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>add17cd7172033014ea272720a33e6066</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setYPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ac40bed004b15ba40e435e00cb851292a</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a21ed597d72116f9ba12aaa67c1d10b4d</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>afd3d082cc93f0d46fa82632405d65adf</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>int64_t</type>
      <name>getOriginalUpdateTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a07e8f65e559a93887bd354895e9334de</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOriginalUpdateTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>af5d4f299d85bf539b9033d8ca7fbe8c7</anchor>
      <arglist>(const int64_t &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>int64_t</type>
      <name>getUpdateTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a3c828906e204100530e5458724bb0616</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setUpdateTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a6dbd5056633f92751b9673d89e358cfe</anchor>
      <arglist>(const int64_t &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>int64_t</type>
      <name>getFirstHopSentTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a61dccec3442ac766a0d44a01d26a7375</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFirstHopSentTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ad36a198bb2d40394e6fc1352bb257f47</anchor>
      <arglist>(const int64_t &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isValid</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a1461f9e93b78ec4ecab7b332bb4516af</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; uint8_t &gt;</type>
      <name>serialize</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a2675e4b6c0b542ed6b27928ce62dba35</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deserialize</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ab1500da5532c9a6691865abb99eb2e1f</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>getSerializedSize</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ac1745d984bc6563070d82c0030674d3e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateTrackId</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a813c43696773bddf5f747cd45ae92252</anchor>
      <arglist>(int32_t value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateXVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a25b7cfba19766f9ea51d85312bf5d5fa</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateYVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a4f659516f39151d2c13e064dacf3f5e2</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateZVelocityECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a495d364ce620fe2b7fa253d19a61fe84</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateXPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>aae44f42340c7a6bd2b59b3641eb0efaa</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateYPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ada016a73863b14aa07ee088e84f41e2b</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateZPositionECEF</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a65c29489550196c3c17cb183b7804bda</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateOriginalUpdateTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a677e0098f4733a9c46c33a7354c035f3</anchor>
      <arglist>(int64_t value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateUpdateTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a5c082b5475891697c2f743795be065ee</anchor>
      <arglist>(int64_t value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateFirstHopSentTime</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a2b4fd571dac5d8ea9961efae9cf13f22</anchor>
      <arglist>(int64_t value) const</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int32_t</type>
      <name>trackId_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a83b38211faf556a04f4b5b5fdfdf37de</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>xVelocityECEF_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ac2276699373de83c96906c4bf98badc4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>yVelocityECEF_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>ae5edb110cce1fd1d142254c6c8fee38d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>zVelocityECEF_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>abc56490366355a4b6f26297326a87226</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>xPositionECEF_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a0a9e6e7a71593b8b848e60e1705df526</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>yPositionECEF_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a640562f2392d84c0537f90f7efc3eb2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>zPositionECEF_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a08a4ab0c0e47a2fedc50271681416b94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int64_t</type>
      <name>originalUpdateTime_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a5c341a17395ffa9918af728c7bcdd9d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int64_t</type>
      <name>updateTime_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a57ee3ddd51a17cd1274aa748d755fe2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int64_t</type>
      <name>firstHopSentTime_</name>
      <anchorfile>df/d02/classdomain_1_1model_1_1ExtrapTrackData.html</anchorfile>
      <anchor>a6aa7312121e756c94e4faf4d379e035e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ExtrapTrackDataTest</name>
    <filename>db/d00/classExtrapTrackDataTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>db/d00/classExtrapTrackDataTest.html</anchorfile>
      <anchor>a5301096f175d663b50012ff97e3e30b5</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>ExtrapTrackData</type>
      <name>validExtrapData_</name>
      <anchorfile>db/d00/classExtrapTrackDataTest.html</anchorfile>
      <anchor>a59ba4577bcdce23dadb30ec6b68e5c27</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter</name>
    <filename>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</filename>
    <base>adapters::IAdapter</base>
    <base>domain::ports::outgoing::IExtrapTrackDataOutgoingPort</base>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackDataZeroMQOutgoingAdapter</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a7eaa0003b6e491825cd62ecea79c8d72</anchor>
      <arglist>(std::unique_ptr&lt; adapters::common::messaging::IMessageSocket &gt; socket)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackDataZeroMQOutgoingAdapter</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>afe049abd4e66bdd74c1c5445f86d7a85</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ExtrapTrackDataZeroMQOutgoingAdapter</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a9e945c59c6ab6fd721a669c5b7d8c210</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>start</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>afb84590a4b7267107faeb27c083231e1</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stop</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a25e31931f783ef0b3cfa788d9f8faacb</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isRunning</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>ab94a621d25ce5b6b9f9203e5b3ca9801</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getName</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>ab30d0da51f15d90486cbaff17dd97d3a</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendExtrapTrackData</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a7abcaada234a99e2f92eacb11e486fdf</anchor>
      <arglist>(const std::vector&lt; domain::model::ExtrapTrackData &gt; &amp;data) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendExtrapTrackData</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>afecb8b06d636f11abb3f24e5220f2df6</anchor>
      <arglist>(const domain::model::ExtrapTrackData &amp;data) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isReady</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a2f4fe7750f60012d6aafe63c55f967c8</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackDataZeroMQOutgoingAdapter</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>ae601c5a7c7df9f54246cc30577bfbc6a</anchor>
      <arglist>(const ExtrapTrackDataZeroMQOutgoingAdapter &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>ExtrapTrackDataZeroMQOutgoingAdapter &amp;</type>
      <name>operator=</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>aa673fc7d0494ab6a6d88eaf8521f91e7</anchor>
      <arglist>(const ExtrapTrackDataZeroMQOutgoingAdapter &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ExtrapTrackDataZeroMQOutgoingAdapter</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a72496035f4f916afb9fe748b12908cd9</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapter &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>ExtrapTrackDataZeroMQOutgoingAdapter &amp;</type>
      <name>operator=</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a6c6d2dbbf483e34efa16b649ce49de0b</anchor>
      <arglist>(ExtrapTrackDataZeroMQOutgoingAdapter &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>loadConfiguration</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>af4a0e302f3eca40826f3af7a4b594d4d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>initializeSocket</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a97f6bac15d665323c6e0e6dc7df0cff9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>publisherWorker</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a2adbc0eddc1492bc43081cdbc9168c1b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>enqueueMessage</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>acc73e629cd936f68637dde5c68a59734</anchor>
      <arglist>(const domain::model::ExtrapTrackData &amp;data)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>endpoint_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a445e4135e585c63f62dc0669799abd55</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>group_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>abbe8b250e23d263c8a825c9d864bb5f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; adapters::common::messaging::IMessageSocket &gt;</type>
      <name>socket_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>aa03ff668e9e563f09b4fe8cee99bf502</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; bool &gt;</type>
      <name>running_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a2fd694e6546154856f3edc434b8e007b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; bool &gt;</type>
      <name>ready_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a08e1bd7adf3a567bda04f59847ac1943</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>ownsSocket_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>afc43d10177da088dfcde93eec08eb689</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::thread</type>
      <name>publisherThread_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a8c4afd4885ddef3c373c447172e5091d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::queue&lt; domain::model::ExtrapTrackData &gt;</type>
      <name>messageQueue_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a803d7940a4df8ffae9c65beef79bacb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::mutex</type>
      <name>queueMutex_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a39f4475c0e20b2150d034e141afa4387</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::condition_variable</type>
      <name>queueCV_</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>acd380f46e2069a768ea91db9984cecb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr int32_t</type>
      <name>REALTIME_THREAD_PRIORITY</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a44bdbdaf513243d8f77a1137a07508bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr int32_t</type>
      <name>DEDICATED_CPU_CORE</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a4cc4fadd12d48c883e0ee5a5600c07d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr std::size_t</type>
      <name>MAX_QUEUE_SIZE</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>ab40bb0d907a2976cf06fc87b6db8e054</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr const char *</type>
      <name>DEFAULT_ENDPOINT</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>a5cfe2fefec8d0e32919eec36887906a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr const char *</type>
      <name>DEFAULT_GROUP</name>
      <anchorfile>d9/d01/classadapters_1_1outgoing_1_1zeromq_1_1ExtrapTrackDataZeroMQOutgoingAdapter.html</anchorfile>
      <anchor>abf474d2ef3ebf7344bbefeabd6491ea1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ExtrapTrackDataZeroMQOutgoingAdapterTest</name>
    <filename>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>aa2d61efe69655772d0bc9836ccf02abb</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>a5bae7c740bdbfa5d661127a045d563f5</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>createAdapter</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>a6a147f9da1d72953ae2851dd4bbc42e1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>ExtrapTrackData</type>
      <name>createTestExtrapTrackData</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>a60936fdd0cf2548d89f98785439c1fc5</anchor>
      <arglist>(int32_t trackId)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; MockMessageSocket &gt;</type>
      <name>mockSocket_</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>a3eca7f702603f513f02643f29e758db9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>MockMessageSocket *</type>
      <name>mockSocketPtr_</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>a53157105aa818f9faed4e4e33734235b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; ExtrapTrackDataZeroMQOutgoingAdapter &gt;</type>
      <name>adapter_</name>
      <anchorfile>d4/d01/classExtrapTrackDataZeroMQOutgoingAdapterTest.html</anchorfile>
      <anchor>a6d21ed61c7f4026f6821661ad29b53c4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::IAdapter</name>
    <filename>dd/d03/classadapters_1_1IAdapter.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~IAdapter</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>a2a8b266a548a0f31dfddba9b8792249d</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>start</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>adb9f3d05459b8a5811d59e573de32e25</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>stop</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>a56f5f108faf8ed3c2f53411af0646258</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>isRunning</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>a7e60830d8497684bca5170789b1ab7b5</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::string</type>
      <name>getName</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>ae26db19919cfaa158fa4ffb73aae8cc3</anchor>
      <arglist>() const noexcept=0</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IAdapter</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>a906c97d5863a13e1ddc4838dcb8eb333</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IAdapter</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>acca1bce7fe9af02b14436c4056696737</anchor>
      <arglist>(const IAdapter &amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IAdapter</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>ae0976175ef5368fc6405fd461a50e91a</anchor>
      <arglist>(IAdapter &amp;&amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>IAdapter &amp;</type>
      <name>operator=</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>a35bce2aba2d2b7e7ccea1b22c50dfbf6</anchor>
      <arglist>(const IAdapter &amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>IAdapter &amp;</type>
      <name>operator=</name>
      <anchorfile>dd/d03/classadapters_1_1IAdapter.html</anchorfile>
      <anchor>ae767c48deb2cd8638a219299989fb056</anchor>
      <arglist>(IAdapter &amp;&amp;)=default</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>domain::ports::outgoing::IExtrapTrackDataOutgoingPort</name>
    <filename>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~IExtrapTrackDataOutgoingPort</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>a95dc65852b2166484bb9b4616c0af27f</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>sendExtrapTrackData</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>aee7f718e0d21bc505acbd1473d803711</anchor>
      <arglist>(const std::vector&lt; model::ExtrapTrackData &gt; &amp;data)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>sendExtrapTrackData</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>ab1685af3583c4ba9e50bd39deb6daea7</anchor>
      <arglist>(const model::ExtrapTrackData &amp;data)=0</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IExtrapTrackDataOutgoingPort</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>ac56b076555f6db990a2bb79cde43d644</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IExtrapTrackDataOutgoingPort</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>a0f9cac13b922f641c8075d9f202ad78c</anchor>
      <arglist>(const IExtrapTrackDataOutgoingPort &amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IExtrapTrackDataOutgoingPort</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>a7ce354e4abf1e0e576c9acc164d3d129</anchor>
      <arglist>(IExtrapTrackDataOutgoingPort &amp;&amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>IExtrapTrackDataOutgoingPort &amp;</type>
      <name>operator=</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>ae0a5c51cb16455e61c366b860da31244</anchor>
      <arglist>(const IExtrapTrackDataOutgoingPort &amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>IExtrapTrackDataOutgoingPort &amp;</type>
      <name>operator=</name>
      <anchorfile>d4/d00/classdomain_1_1ports_1_1outgoing_1_1IExtrapTrackDataOutgoingPort.html</anchorfile>
      <anchor>a4e46335465ef23706927efdad7b5e1d3</anchor>
      <arglist>(IExtrapTrackDataOutgoingPort &amp;&amp;)=default</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::common::messaging::IMessageSocket</name>
    <filename>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~IMessageSocket</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a2de0b98aa21970b141214f9e36eea91c</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>connect</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a2e0991fc5bd0d9f8ac4c67e4dfdeb3a9</anchor>
      <arglist>(const std::string &amp;endpoint)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>send</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a01c590d5a84b5411a46a2085a54ad600</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>send</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a1628f518bd96050431676c5cb8ef69b3</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data, const std::string &amp;group)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::optional&lt; std::vector&lt; uint8_t &gt; &gt;</type>
      <name>receive</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>ac53d1aa1d939269beaf2f0497566e8b1</anchor>
      <arglist>(int32_t timeoutMs)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>close</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>afa635a41ea22407e42f56d3b4951e420</anchor>
      <arglist>() noexcept=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>isConnected</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>ac738b8c92f09c76cdcee59ce020c686c</anchor>
      <arglist>() const noexcept=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::string</type>
      <name>getSocketType</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a0702ab897b91019c7387e9b6f7010253</anchor>
      <arglist>() const noexcept=0</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IMessageSocket</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a8096cc503f6ce8d7eeb1a0ac5e11601c</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IMessageSocket</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a0a780b5291d0e8a35ea98ff1ca644b6e</anchor>
      <arglist>(const IMessageSocket &amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>IMessageSocket</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a273737081a27fcaf8d3becf6ca458992</anchor>
      <arglist>(IMessageSocket &amp;&amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>IMessageSocket &amp;</type>
      <name>operator=</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a9d8e2849058fe245581743c4c75f6ea9</anchor>
      <arglist>(const IMessageSocket &amp;)=default</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>IMessageSocket &amp;</type>
      <name>operator=</name>
      <anchorfile>d1/d03/classadapters_1_1common_1_1messaging_1_1IMessageSocket.html</anchorfile>
      <anchor>a478b39d89d1f33ab07d70ca459b4fae7</anchor>
      <arglist>(IMessageSocket &amp;&amp;)=default</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>domain::ports::incoming::ITrackDataIncomingPort</name>
    <filename>da/d03/classdomain_1_1ports_1_1incoming_1_1ITrackDataIncomingPort.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ITrackDataIncomingPort</name>
      <anchorfile>da/d03/classdomain_1_1ports_1_1incoming_1_1ITrackDataIncomingPort.html</anchorfile>
      <anchor>aef64e043c3be8ad1c2b648c79db6d7b5</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>processAndForwardTrackData</name>
      <anchorfile>da/d03/classdomain_1_1ports_1_1incoming_1_1ITrackDataIncomingPort.html</anchorfile>
      <anchor>a68caa10cd40d4a198a6be76a108c672c</anchor>
      <arglist>(const model::TrackData &amp;trackData)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Logger</name>
    <filename>db/d03/classLogger.html</filename>
    <member kind="function">
      <type>void</type>
      <name>initialize</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a42b4030f70fe2ff836edc41131deebe4</anchor>
      <arglist>(const std::string &amp;name=&quot;a_hexagon&quot;, const std::string &amp;logFile=&quot;&quot;, bool async=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>shutdown</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a29c2ff4aa380cbf55c135b93eaf5d61f</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInitialized</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a4f65d1c1dd95a29513e5cd1d887f376d</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>info</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a1537d6dc2cc75e2787df3c5882e929e6</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>debug</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a3583d5871c4bed96b24df5de65b193d9</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>warn</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>abca590d09e3faf293231585db9e678dc</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>error</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>ad4013374e529f80c31b0356be5778f4c</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>critical</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a3123c7b0d401a889501e215458fb7661</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trace</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a2531ec29f148be6314a0ed492097d5ff</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Logger &amp;</type>
      <name>getInstance</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a82e2d739067944245c6ae4710439a5f6</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>Logger</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>ac45b6ef7069b222c094da3724d1aa8fb</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>~Logger</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a81414467aac1f8740c5bd13624f3aa0b</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>Logger</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>add0f34d4c753c86af8074840a72366b3</anchor>
      <arglist>(const Logger &amp;)=delete</arglist>
    </member>
    <member kind="function" protection="private">
      <type>Logger &amp;</type>
      <name>operator=</name>
      <anchorfile>db/d03/classLogger.html</anchorfile>
      <anchor>a8c0ca770cdc91ca9fc3015286568e1ea</anchor>
      <arglist>(const Logger &amp;)=delete</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>utils::Logger</name>
    <filename>d2/d02/classutils_1_1Logger.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Level</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>af342aa408b27c6290eeebf0b6e4b8a8f</anchor>
      <arglist></arglist>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8fadd4ec0ac4e58f7c32a01244ae91150b1">Trace</enumvalue>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8faa603905470e2a5b8c13e96b579ef0dba">Debug</enumvalue>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8fa4059b0251f66a18cb56f544728796875">Info</enumvalue>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8fa56525ae64d370c0b448ac0d60710ef17">Warn</enumvalue>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8fa902b0d55fddef6f8d651fe1035b7d4bd">Error</enumvalue>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8fa278d01e5af56273bae1bb99a98b370cd">Critical</enumvalue>
      <enumvalue file="d2/d02/classutils_1_1Logger.html" anchor="af342aa408b27c6290eeebf0b6e4b8a8fad15305d7a4e34e02489c74a5ef542f36">Off</enumvalue>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>init</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a894382020336e2fa814f4611148c8cd7</anchor>
      <arglist>(const std::string &amp;app_name=&quot;a_hexagon&quot;, std::size_t queue_size=8192, std::size_t thread_count=1)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>initWithFile</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>aa1b3a68b450b98a82c1426fb4a0153c3</anchor>
      <arglist>(const std::string &amp;app_name, const std::string &amp;log_file, std::size_t max_file_size=5 *1024 *1024, std::size_t max_files=3)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>setLevel</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>ae02310bf9d5ed6334a357ba1f14bff3c</anchor>
      <arglist>(Level level)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>shutdown</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a90c64228f55a9ea0da67edca275b0804</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>trace</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a7da281dd6ecb1ec246c2b5a74cc5534a</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>debug</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a4f8a5580a736b963db9d58bd427f0858</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>info</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>ac819da3d70613ec1a7631625892ca404</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>warn</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a33cd4e2a6ede3181c6f3c906279180ac</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>error</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a98a01c82e7f5072a845bd5af7c1b00bc</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>critical</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>ae450f0e5bb5960af2b7962e6dee15e11</anchor>
      <arglist>(spdlog::format_string_t&lt; Args... &gt; fmt, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>logLatency</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a196e4b67b48ebca06bd3ed6cd0655ea8</anchor>
      <arglist>(const std::string &amp;component, const std::string &amp;operation, int64_t latency_us)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>logTrackReceived</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a48b0345bdc6715767f0f5fd643af6efb</anchor>
      <arglist>(int32_t track_id, int64_t hop_latency)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>logTrackExtrapolated</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a359216b45bd0258438259eeb18a18d37</anchor>
      <arglist>(int32_t track_id, int extrap_count, int64_t latency_us)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>isInitialized</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>aba4dfb4fa0860d1fe1a2fe68526146c3</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>Logger</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a61fc07099163ba2b907d709616898599</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>~Logger</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a989789e1cd0ecbf3509d581318f0c97e</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>Logger</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>ae336927bfdd8dc5ecc1fdb2eb092b98a</anchor>
      <arglist>(const Logger &amp;)=delete</arglist>
    </member>
    <member kind="function" protection="private">
      <type>Logger &amp;</type>
      <name>operator=</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a6d819db78b6f57a34edb6910dc60f48b</anchor>
      <arglist>(const Logger &amp;)=delete</arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static bool</type>
      <name>initialized_</name>
      <anchorfile>d2/d02/classutils_1_1Logger.html</anchorfile>
      <anchor>a05d945b4997f85e6ca604476524d8e03</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LoggerTest</name>
    <filename>d2/d03/classLoggerTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>d2/d03/classLoggerTest.html</anchorfile>
      <anchor>a19c30cd9daa5e93462a4b8470574f4e8</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>d2/d03/classLoggerTest.html</anchorfile>
      <anchor>af011febd3a83ddf3d7c3825d818b1994</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::MessagePipeline</name>
    <filename>dd/d00/classadapters_1_1MessagePipeline.html</filename>
    <member kind="function">
      <type></type>
      <name>MessagePipeline</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a68aa08946bb3fbc4c2755e7d1a886cc5</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MessagePipeline</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>af9d87fbd6e323065fa0355ba3d8da5a0</anchor>
      <arglist>(std::string name)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MessagePipeline</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>ad961460b0811f3af03e0265088b671af</anchor>
      <arglist>(std::string name, std::shared_ptr&lt; IAdapter &gt; incoming, std::shared_ptr&lt; IAdapter &gt; outgoing=nullptr)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MessagePipeline</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>ad685943aba71989ef343d1f809585ba9</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MessagePipeline</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a86da1a20dab10eee80f5912f1a4e633f</anchor>
      <arglist>(MessagePipeline &amp;&amp;)=default</arglist>
    </member>
    <member kind="function">
      <type>MessagePipeline &amp;</type>
      <name>operator=</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a7e64591cb3de09eda2d01657b3263237</anchor>
      <arglist>(MessagePipeline &amp;&amp;)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MessagePipeline</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a0362aa0bca749e02deb3ed78cd4818b0</anchor>
      <arglist>(const MessagePipeline &amp;)=default</arglist>
    </member>
    <member kind="function">
      <type>MessagePipeline &amp;</type>
      <name>operator=</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>aa674f03d4c363f254b03d3621d06c477</anchor>
      <arglist>(const MessagePipeline &amp;)=default</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>start</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a0219faa240cf25a504459aad4e7de76c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stop</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a1ad9bb260b12e600a412786d92e468c7</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isRunning</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>abbf566ff2b15093960437510ee842413</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>getName</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a122eb6478f133f76d6139ff8ba6bdc0a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a76f7b4e8756d24e56754da6c5fd75f56</anchor>
      <arglist>(std::string name)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; IAdapter &gt;</type>
      <name>getIncomingAdapter</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>af116fb49584a447bbc7662c838d629cf</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setIncomingAdapter</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a05cb4c4ae32ae902d6e5a11bf1deae0e</anchor>
      <arglist>(std::shared_ptr&lt; IAdapter &gt; adapter)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; IAdapter &gt;</type>
      <name>getOutgoingAdapter</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a6477ea92467d4b72ea986cb2ae9a9e73</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOutgoingAdapter</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a5def558260f739ac555f0b6b4cfe9782</anchor>
      <arglist>(std::shared_ptr&lt; IAdapter &gt; adapter)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasIncomingAdapter</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>ad57419281256aff3c74c480f4ae3fb26</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasOutgoingAdapter</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a98d008a4fad53bd73a70e93d50dbc70d</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>name_</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a5b74e4d780ef9feb406fc50b937fe8e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::shared_ptr&lt; IAdapter &gt;</type>
      <name>incomingAdapter_</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a71625596346df4a440261574f3b9a7b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::shared_ptr&lt; IAdapter &gt;</type>
      <name>outgoingAdapter_</name>
      <anchorfile>dd/d00/classadapters_1_1MessagePipeline.html</anchorfile>
      <anchor>a86dc6119c953bdcf449048bb355c9f8a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MockAdapter</name>
    <filename>de/d01/classMockAdapter.html</filename>
    <base>adapters::IAdapter</base>
    <member kind="function">
      <type></type>
      <name>MOCK_METHOD</name>
      <anchorfile>de/d01/classMockAdapter.html</anchorfile>
      <anchor>aff793c66733042a35313377b303e7545</anchor>
      <arglist>(bool, start,(),(override))</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MOCK_METHOD</name>
      <anchorfile>de/d01/classMockAdapter.html</anchorfile>
      <anchor>a11148c532348f32183474857dfc69db0</anchor>
      <arglist>(void, stop,(),(override))</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MOCK_METHOD</name>
      <anchorfile>de/d01/classMockAdapter.html</anchorfile>
      <anchor>ac345ee1f4f08a438db8940170242aabd</anchor>
      <arglist>(bool, isRunning,(),(const, override))</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MOCK_METHOD</name>
      <anchorfile>de/d01/classMockAdapter.html</anchorfile>
      <anchor>a7adfdf46998975c5ff77a61284604884</anchor>
      <arglist>(std::string, getName,(),(const, noexcept, override))</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>a_hexagon::test::mocks::MockIncomingPort</name>
    <filename>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</filename>
    <base>domain::ports::incoming::ITrackDataIncomingPort</base>
    <member kind="function">
      <type></type>
      <name>MockIncomingPort</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a84cc357b5ce6cc014d31526e63ff9e60</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MockIncomingPort</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a186fa2a0ac8b194b803c7be853555c79</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAndForwardTrackData</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a1cc1977c06a0debc27c039303d42a8ee</anchor>
      <arglist>(const domain::model::TrackData &amp;trackData) override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getReceivedTrackDataCount</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a72d946b09f4209c6ffcaae283a4b571b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; domain::model::TrackData &gt;</type>
      <name>getLastReceivedTrackData</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a2737ed04088c8724e9939fe8b94044ce</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; domain::model::TrackData &gt;</type>
      <name>getAllReceivedTrackData</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>aea62302d3ff9e7ba5fc0d78cdedb3726</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; domain::model::TrackData &gt;</type>
      <name>getReceivedTrackDataAt</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>ae28706b61883ad6caf657b166f80c1ac</anchor>
      <arglist>(size_t index) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getProcessCallCount</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a5d628644bb8c7933ad6d591c89f099d5</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a022365a08f3df6f94457b8e4cf1a5a27</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>waitForMessages</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a213829e9222b17b1f2cea33ffc79ea22</anchor>
      <arglist>(int expectedCount, int timeoutMs) const</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MockIncomingPort</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a8a1bd0b719cd4713160789f5e311550f</anchor>
      <arglist>(const MockIncomingPort &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>MockIncomingPort &amp;</type>
      <name>operator=</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>acb08940d5e9bdb39b4e5e3339cd02884</anchor>
      <arglist>(const MockIncomingPort &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MockIncomingPort</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>ad19554be7cb3fced55e0d3d22d9af803</anchor>
      <arglist>(MockIncomingPort &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>MockIncomingPort &amp;</type>
      <name>operator=</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a43d1b534b1b8996b2fb2463eb008ce7d</anchor>
      <arglist>(MockIncomingPort &amp;&amp;)=delete</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::mutex</type>
      <name>mutex_</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>aa388509903db6c1ca4e2197f4c242beb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; domain::model::TrackData &gt;</type>
      <name>receivedTrackData_</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a0521e45c16431301a3030d0331ce68e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>processCallCount_</name>
      <anchorfile>d4/d02/classa__hexagon_1_1test_1_1mocks_1_1MockIncomingPort.html</anchorfile>
      <anchor>a13cea1cfa192408d0b103bb3de53edde</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>a_hexagon::test::mocks::MockMessageSocket</name>
    <filename>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</filename>
    <base>adapters::common::messaging::IMessageSocket</base>
    <class kind="struct">a_hexagon::test::mocks::MockMessageSocket::SentMessage</class>
    <member kind="function">
      <type></type>
      <name>MockMessageSocket</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>acac83e4973749ea708a44f5243821ab8</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MockMessageSocket</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>afa768baced8358a17ece479063b0aa66</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>connect</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a008d90bd179232d6f32b35ae528a0c8e</anchor>
      <arglist>(const std::string &amp;endpoint) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>send</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a6fbd9e9879921d3a3b4ef6fd76692e10</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>send</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>aa8113213d6a472a6cb228e726c964eea</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data, const std::string &amp;group) override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; std::vector&lt; uint8_t &gt; &gt;</type>
      <name>receive</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a324106da31fdf236c77d3b8c1687a21c</anchor>
      <arglist>(int32_t timeoutMs) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a78650c0620392865d94ba4b598ba5a40</anchor>
      <arglist>() noexcept override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ad09ec4a8f5de86c9283c21c839ad80f8</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getSocketType</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a08388c2a6826afdbc2df88760edff6d1</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>queueIncomingMessage</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a3dbf97d0186d41c5c8bc79eb97cce71c</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>queueIncomingMessages</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ab0afcac4e45b3c5cb6cd68cfd132a1c7</anchor>
      <arglist>(const std::vector&lt; std::vector&lt; uint8_t &gt; &gt; &amp;messages)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getIncomingMessageCount</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a0ba26a2544ac45af479343cc14589320</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::queue&lt; SentMessage &gt;</type>
      <name>getSentMessages</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>aac120e23065843037709635d0ee95f74</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getSentMessageCount</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ace282d80adecdd827d2695a717c175ca</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; SentMessage &gt;</type>
      <name>getLastSentMessage</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a1bbc98103ed4300c7150a075f297e12a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ae70f1176400115a2c5c3a77d9e332a74</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSendCallCount</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ad6b53a708b6310da229c51dd565a4f37</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getReceiveCallCount</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a398bb62970f68011e676e50a34dc6c86</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getConnectCallCount</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>afbd98d5beddd1d8a9694a04cbfe0ef53</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getCloseCallCount</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ad8a29fb7cc4aa6bfa88d5d5a43acb366</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getEndpoint</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a14bb1997d477f958a420fde584bf98cd</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFailConnect</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a871cc8e5ca670d34c363f81355bbb5fe</anchor>
      <arglist>(bool fail)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFailSend</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a2edaa1060907b9a6e911e463d3622d06</anchor>
      <arglist>(bool fail)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>simulateDisconnect</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a710e46e131906cb6405f183a8a7494da</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MockMessageSocket</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a6b19edeb30bf19e1003c942d5fa3a8f0</anchor>
      <arglist>(const MockMessageSocket &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>MockMessageSocket &amp;</type>
      <name>operator=</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a0293c0ec0e2ca8c392cc70c02648d397</anchor>
      <arglist>(const MockMessageSocket &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MockMessageSocket</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a2d7d945b0b0d4d65cf059083a994de1d</anchor>
      <arglist>(MockMessageSocket &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>MockMessageSocket &amp;</type>
      <name>operator=</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>afc271c1f3f3a6cc6db8d514144b4f79c</anchor>
      <arglist>(MockMessageSocket &amp;&amp;)=delete</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::mutex</type>
      <name>mutex_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a8a1bed612a2a0a970f0d6b1b7a84c1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::condition_variable</type>
      <name>cv_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a43cccd391712158db1361cd4c114b06b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>connected_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>aa93af9bf756fa829fc6ae74bee17280b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>endpoint_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>aa31000dca349483b84e3e8376762081b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::queue&lt; std::vector&lt; uint8_t &gt; &gt;</type>
      <name>incomingMessages_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a9da13544985c86d2589aa9ab2e8f8bb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::queue&lt; SentMessage &gt;</type>
      <name>sentMessages_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ac6e063ab67ab55ee2b9e09e17e984ca1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>sendCallCount_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>aa15097ade336932bc4d47be044aed8a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>receiveCallCount_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a109371d8eeb0cf0fe96e6a00e7e9da32</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>connectCallCount_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>ae03883d567cb35673ab83ca9500efdca</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>closeCallCount_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>adffd0bda93f99a98ffcf10ebd3456843</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>failConnect_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a182be8c4950dc45dbe643a91a9cb4f41</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>failSend_</name>
      <anchorfile>d5/d00/classa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket.html</anchorfile>
      <anchor>a588c09df324fcb7522883508f281c4d9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>a_hexagon::test::mocks::MockOutgoingPort</name>
    <filename>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</filename>
    <base>domain::ports::outgoing::IExtrapTrackDataOutgoingPort</base>
    <class kind="struct">a_hexagon::test::mocks::MockOutgoingPort::SentRecord</class>
    <member kind="function">
      <type></type>
      <name>MockOutgoingPort</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>acd330c67124f1f691f639c527483f86e</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MockOutgoingPort</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>ae612e67b2053cd05ed26e7ab27bc53ca</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendExtrapTrackData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>aab8a5825c429e5f20666cccf9d2b36e6</anchor>
      <arglist>(const std::vector&lt; domain::model::ExtrapTrackData &gt; &amp;data) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendExtrapTrackData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a89fab35459e3433ece02d2d54a84cce7</anchor>
      <arglist>(const domain::model::ExtrapTrackData &amp;data) override</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getSentDataCount</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a438564167354b909bb1395129bab7260</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; domain::model::ExtrapTrackData &gt;</type>
      <name>getAllSentData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a1bb446123488d53c7cd23a2324a41211</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; SentRecord &gt;</type>
      <name>getAllSentRecords</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>ae8bf534be594eb0cee42f32ab3f14907</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; domain::model::ExtrapTrackData &gt;</type>
      <name>getSentDataAt</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a305c0b593be0fb87166761661e9aaa33</anchor>
      <arglist>(size_t index) const</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; domain::model::ExtrapTrackData &gt;</type>
      <name>getLastSentData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>ae76dd4ca4352019a519a715695dd2503</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; domain::model::ExtrapTrackData &gt;</type>
      <name>getFirstSentData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>af3f5809c908c9d362fe75a233b317bf1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; domain::model::ExtrapTrackData &gt;</type>
      <name>getSentDataByTrackId</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a41d419eb725a2d76d9cf4b17e4037724</anchor>
      <arglist>(int32_t trackId) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSendSingleCallCount</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a022289fc4b6586b338db7afb2d52e2e3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSendVectorCallCount</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a34de709dd3a150a7e798f7ed7a3aacc7</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getTotalSendCallCount</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a983b7310905d74b59093bcfdc1512381</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getFailedSendCount</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a976cf7b6e4ab9b5cb3424c43a2ba77f3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>waitForSentData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>af02618ac13842cf5c537b7513f2d183c</anchor>
      <arglist>(size_t expectedCount, int timeoutMs) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>waitForAnySentData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>af7e69eb36e9fb03a8d44d3ec85250708</anchor>
      <arglist>(int timeoutMs) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFailSend</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>af876f7081cf71a15254d0ad4cb1e97bd</anchor>
      <arglist>(bool fail)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isFailSendEnabled</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a96738ce42f2ff396ba8a5913634861f4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a98b04a7b892bc2ad11a88c1e571d0279</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearSentData</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a389247b8bf37c70835003b276ca6c100</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getAverageSendIntervalUs</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a8605083a61246ce62a0f289877a495bd</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>verifyLinearExtrapolation</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a031d6443ba49a5a98bdf1904c9eaee0e</anchor>
      <arglist>(double originalPosition, double velocity, double tolerance=0.001) const</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MockOutgoingPort</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>ae8fab026a4fe1cfeab53fb4a10cc9e01</anchor>
      <arglist>(const MockOutgoingPort &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>MockOutgoingPort &amp;</type>
      <name>operator=</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>afd10668ecb39c2c85de3934793b5a339</anchor>
      <arglist>(const MockOutgoingPort &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MockOutgoingPort</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a55d243fa2b8fcf8fd1b5aa08f6ebcf29</anchor>
      <arglist>(MockOutgoingPort &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>MockOutgoingPort &amp;</type>
      <name>operator=</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>aeced49dfef0711dedd156a1808a83e10</anchor>
      <arglist>(MockOutgoingPort &amp;&amp;)=delete</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::mutex</type>
      <name>mutex_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>abda23ccf3d5c46b118dfc4e4b9eddd10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::condition_variable</type>
      <name>cv_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a7bbae6927068292d455b82f15006fa63</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; SentRecord &gt;</type>
      <name>sentData_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a1cd1687a7597cedcb82bfaf3bb846b0a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>sendSingleCallCount_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a7ca55d706fb2e65e5d4f0c3b2b6afaeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>sendVectorCallCount_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a4f6104ba9f8c632a15a870d71eca2b2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>failedSendCount_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>a41030c767709bfa0159cb190e93703d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>sequenceCounter_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>af692243a839a37401cc314fbe35b65a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>failSend_</name>
      <anchorfile>de/d01/classa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort.html</anchorfile>
      <anchor>ab319267fac956ef0ba702abf45371c1e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>a_hexagon::test::mocks::MockMessageSocket::SentMessage</name>
    <filename>db/d02/structa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket_1_1SentMessage.html</filename>
    <member kind="variable">
      <type>std::vector&lt; uint8_t &gt;</type>
      <name>data</name>
      <anchorfile>db/d02/structa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket_1_1SentMessage.html</anchorfile>
      <anchor>a1a2ddd5d19c9a5e4ff7aa6fed6472c03</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>group</name>
      <anchorfile>db/d02/structa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket_1_1SentMessage.html</anchorfile>
      <anchor>a50c7e87c5e07627fb0492213da099193</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::chrono::steady_clock::time_point</type>
      <name>timestamp</name>
      <anchorfile>db/d02/structa__hexagon_1_1test_1_1mocks_1_1MockMessageSocket_1_1SentMessage.html</anchorfile>
      <anchor>a24d11928d492bb598e2377e8d53ee624</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>a_hexagon::test::mocks::MockOutgoingPort::SentRecord</name>
    <filename>db/d03/structa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort_1_1SentRecord.html</filename>
    <member kind="variable">
      <type>domain::model::ExtrapTrackData</type>
      <name>data</name>
      <anchorfile>db/d03/structa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort_1_1SentRecord.html</anchorfile>
      <anchor>aef9e9554380068d041378fc2950cc2a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::chrono::steady_clock::time_point</type>
      <name>sentTime</name>
      <anchorfile>db/d03/structa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort_1_1SentRecord.html</anchorfile>
      <anchor>a127521df9a4f7b7f6b41b505d80395bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>sequenceNumber</name>
      <anchorfile>db/d03/structa__hexagon_1_1test_1_1mocks_1_1MockOutgoingPort_1_1SentRecord.html</anchorfile>
      <anchor>a741a0c3f2a500b71383985d991096e0c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>domain::model::TrackData</name>
    <filename>dc/d00/classdomain_1_1model_1_1TrackData.html</filename>
    <member kind="function">
      <type></type>
      <name>TrackData</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>afbd357ad01c63032b71eb34876e23bbd</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackData</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>aa8e46745e7052e68c2e8675cc840fa3d</anchor>
      <arglist>(const TrackData &amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackData</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a46b4e1ae396933055e5a0db5c5051c0a</anchor>
      <arglist>(TrackData &amp;&amp;other) noexcept=default</arglist>
    </member>
    <member kind="function">
      <type>TrackData &amp;</type>
      <name>operator=</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a8721dcef8086cdfca3ba05e498ca8362</anchor>
      <arglist>(const TrackData &amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type>TrackData &amp;</type>
      <name>operator=</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a8860a6d0f1c75b226914f1984d847e2f</anchor>
      <arglist>(TrackData &amp;&amp;other) noexcept=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TrackData</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a733ee16cd7cad327d0e75b0b5036ab8c</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>getTrackId</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a76036a48062bcef72e758a8a873b9a35</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTrackId</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a14b48474d7f39bd646bcf85567eaa6c5</anchor>
      <arglist>(const int32_t &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getXVelocityECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>aad987a4546c712d6e67f9ba358eece3f</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setXVelocityECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a3302bb0db0a48b69a9dea0843254344a</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getYVelocityECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>af77195063d065c5f0d7dc5b63f88363d</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setYVelocityECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a3a44e52ee923bb135992b14711725000</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZVelocityECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a26671293a9ae314eb57839ecd2c8b23a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZVelocityECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a082cafc85546c369eb2271dd0f4d0c9e</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getXPositionECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a6558be09f739b2f1fc847f942aabf5e8</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setXPositionECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>acaddc6c98e571d5c764d5efe4334ba35</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getYPositionECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a1c348cb2c69b2ad4da111d1650cb570b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setYPositionECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a43ac6f94fda7366c4daab8542083819c</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getZPositionECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a72dfadaf180449a3a8c644b0d1c7e35a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZPositionECEF</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>ac1593e6605994fd75bac66ffe55164b0</anchor>
      <arglist>(const double &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>int64_t</type>
      <name>getOriginalUpdateTime</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a6511bf559bc53b277b2801e6b56819e4</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOriginalUpdateTime</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a9d8b4329f6061d2c4aabdbca5f43da61</anchor>
      <arglist>(const int64_t &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isValid</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>abba5bbbdec7116c3af7fd9f15ffe847e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; uint8_t &gt;</type>
      <name>serialize</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a692051411fa0f604517b75f80b24b618</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deserialize</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>aac1990f4a8b98f0cc03ee5fe25399b9c</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>getSerializedSize</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a995a6d40e2a848bf51676fabd3fbc68c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateTrackId</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>aa243bbbfe931728e3682f582d16f465e</anchor>
      <arglist>(int32_t value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validatePosition</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a16bc8d428886863ba37b6289e8090bd2</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateVelocity</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a6f35effa78946cfb116047a2e0a2bdd0</anchor>
      <arglist>(double value) const</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>validateTimestamp</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a700ffaffc20720424adc5ec36589407a</anchor>
      <arglist>(int64_t value) const</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int32_t</type>
      <name>trackId_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>ab183881dd1c728ff1ce94772bb14fd95</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>xVelocityECEF_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a0fb9c546c28d8883dfe481c9126cbc7a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>yVelocityECEF_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a05781cda00466745810e1f853c14298a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>zVelocityECEF_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a3e1127d713081e09f77f704cbe8bc202</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>xPositionECEF_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a21e7db2fdb363aeca6fe43238d1f2c12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>yPositionECEF_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>ad67162fe90966b82f18150ecd92b5566</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>zPositionECEF_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>ac98d43afda3681099d1e4c5c3f462e93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int64_t</type>
      <name>originalUpdateTime_</name>
      <anchorfile>dc/d00/classdomain_1_1model_1_1TrackData.html</anchorfile>
      <anchor>a56ea181dd50afa6929f073166d8bb139</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>domain::logic::TrackDataExtrapolator</name>
    <filename>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</filename>
    <base>domain::ports::incoming::ITrackDataIncomingPort</base>
    <member kind="function">
      <type></type>
      <name>TrackDataExtrapolator</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>ab693ff76272334b340bdc25b00d669fa</anchor>
      <arglist>(std::unique_ptr&lt; domain::ports::outgoing::IExtrapTrackDataOutgoingPort &gt; outgoingPort)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataExtrapolator</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>aed52ee3df02589ba90f64c1401e7f46f</anchor>
      <arglist>(domain::ports::outgoing::IExtrapTrackDataOutgoingPort *outgoingPort)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAndForwardTrackData</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>a4af9555dd108a446e4b16d416fcc7360</anchor>
      <arglist>(const domain::model::TrackData &amp;trackData)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>extrapolateTrackData</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>ac273b01129efc99c894080a7134d26a0</anchor>
      <arglist>(const TrackData &amp;trackData, double inputFrequency, double outputFrequency)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataExtrapolator</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>a460022461c0343f9e9d4f52ffb73a77d</anchor>
      <arglist>(const TrackDataExtrapolator &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>TrackDataExtrapolator &amp;</type>
      <name>operator=</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>af707c64e8a7fd824c49bb20cfc8fd884</anchor>
      <arglist>(const TrackDataExtrapolator &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataExtrapolator</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>a78e8174b3b3d0b8fda28d3e084f19d06</anchor>
      <arglist>(TrackDataExtrapolator &amp;&amp;)=default</arglist>
    </member>
    <member kind="function">
      <type>TrackDataExtrapolator &amp;</type>
      <name>operator=</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>a3cf3a1a88d45f02e66e3a9700adb48bd</anchor>
      <arglist>(TrackDataExtrapolator &amp;&amp;)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TrackDataExtrapolator</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>a65980457482706670d83a34c7d140535</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; domain::ports::outgoing::IExtrapTrackDataOutgoingPort &gt;</type>
      <name>outgoingPort_</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>aa39f583f81caee6a4d88f41b5e74270b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>domain::ports::outgoing::IExtrapTrackDataOutgoingPort *</type>
      <name>rawOutgoingPort_</name>
      <anchorfile>db/d00/classdomain_1_1logic_1_1TrackDataExtrapolator.html</anchorfile>
      <anchor>af95aff3a299858b5abb9e9a27f199def</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TrackDataExtrapolatorTest</name>
    <filename>d2/d01/classTrackDataExtrapolatorTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>accb4e3d1b0b42544ff9ff6a31219f99f</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>acb3742202cf73f582e3c67b69439832a</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>createExtrapolator</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>a87c49a60a95de09be90243eb516274c2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>TrackData</type>
      <name>createTestTrackData</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>a7654d81022bfda41650429b5ea3a7b4b</anchor>
      <arglist>(int32_t trackId)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; MockOutgoingPort &gt;</type>
      <name>mockOutgoingPort_</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>a4f1b377c9f0dc450b4046e602f4bea84</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>MockOutgoingPort *</type>
      <name>mockOutgoingPortPtr_</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>ab5fb8b37861a7aac04365c2006955cb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; TrackDataExtrapolator &gt;</type>
      <name>extrapolator_</name>
      <anchorfile>d2/d01/classTrackDataExtrapolatorTest.html</anchorfile>
      <anchor>a45f98d7427f1d88cda8579781cea1215</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TrackDataTest</name>
    <filename>d0/d00/classTrackDataTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>d0/d00/classTrackDataTest.html</anchorfile>
      <anchor>ac4c712b6375201d546bda62b8f516cf6</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>TrackData</type>
      <name>validTrackData_</name>
      <anchorfile>d0/d00/classTrackDataTest.html</anchorfile>
      <anchor>a1d82038b83bbb3e50bbc3770ba09a8d0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter</name>
    <filename>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</filename>
    <base>adapters::IAdapter</base>
    <member kind="function">
      <type></type>
      <name>TrackDataZeroMQIncomingAdapter</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a7879541ee08db27604474c96dc0fe44a</anchor>
      <arglist>(std::shared_ptr&lt; domain::ports::incoming::ITrackDataIncomingPort &gt; incomingPort, std::unique_ptr&lt; adapters::common::messaging::IMessageSocket &gt; socket)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataZeroMQIncomingAdapter</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a5c251ed45b50f0e7fc10dc614bab1253</anchor>
      <arglist>(domain::ports::incoming::ITrackDataIncomingPort *incomingPort)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataZeroMQIncomingAdapter</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a13af47fb1399618c71d6f434465d6f97</anchor>
      <arglist>(std::shared_ptr&lt; domain::ports::incoming::ITrackDataIncomingPort &gt; incomingPort)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TrackDataZeroMQIncomingAdapter</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a235d2e39df287f3244b9e294ab21f982</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>start</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a19b3ef73e13d71c1b6dd66dcebc145ee</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stop</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a7ab906679a646aa730ba3f0951d49603</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isRunning</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a3845cc0727d21540fbd38ee84975ca6d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getName</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>acdc6659e11389e9f69f977e019bbb647</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataZeroMQIncomingAdapter</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>adb43d0da9b73994eb97902be9d15c0a2</anchor>
      <arglist>(const TrackDataZeroMQIncomingAdapter &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>TrackDataZeroMQIncomingAdapter &amp;</type>
      <name>operator=</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a783fcc5647d0e1416ea312c2a429ffed</anchor>
      <arglist>(const TrackDataZeroMQIncomingAdapter &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TrackDataZeroMQIncomingAdapter</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a10921306bba2fcec4f429bc34febe9d1</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapter &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>TrackDataZeroMQIncomingAdapter &amp;</type>
      <name>operator=</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a33f63b9161d86013b3d79b4aee46e10c</anchor>
      <arglist>(TrackDataZeroMQIncomingAdapter &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>loadConfiguration</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a0663f13de6bf049d9fc0fc2cf7e95995</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>initializeSocket</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a777051c2fe018960331768287ee56e08</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>receiveLoop</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a5f1c5efb3c8c5a7bfd2f59affacb92cb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>deserializeTrackData</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a7ef13f5aedb327484ec80bdf438d982d</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data, domain::model::TrackData &amp;trackData)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>endpoint_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>ac5ad1f758b2f8b999b108734f229e68c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int32_t</type>
      <name>receiveTimeout_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>ae68be7aec51dad9978a14dbc3ecdf8b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; adapters::common::messaging::IMessageSocket &gt;</type>
      <name>socket_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a1627acc1cacebcab0d40720fec4b5cdb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::thread</type>
      <name>receiveThread_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>ab90f81261279021a83d1b6eed1860875</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; bool &gt;</type>
      <name>running_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>ab7e0ffb84323633bb55340a7c3e34364</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; bool &gt;</type>
      <name>stopRequested_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a89b8ed596b65afe08bc5ac68f817e90a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>domain::ports::incoming::ITrackDataIncomingPort *</type>
      <name>incomingPort_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a2084086513bb93c28504f3e1a0cd4911</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::shared_ptr&lt; domain::ports::incoming::ITrackDataIncomingPort &gt;</type>
      <name>incomingPortShared_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a3e6cf627c06404b3cb220c1fef19d44e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>ownsSocket_</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a54afa2b864c864a426e938e4d0730ebb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr const char *</type>
      <name>DEFAULT_ENDPOINT</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>ab3c38a1de81e984e8bbd5da2e424ed92</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr const char *</type>
      <name>DEFAULT_GROUP</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a0fa7400c4f108d9cc96ef2257f90f212</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr int32_t</type>
      <name>DEFAULT_RECEIVE_TIMEOUT</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>af8e870f48fb569bc7b9ecd5c45ac40e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr int32_t</type>
      <name>REALTIME_THREAD_PRIORITY</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>a667f61fbc12e1fe64c187efc3d53f3d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private" static="yes">
      <type>static constexpr int32_t</type>
      <name>DEDICATED_CPU_CORE</name>
      <anchorfile>d6/d03/classadapters_1_1incoming_1_1zeromq_1_1TrackDataZeroMQIncomingAdapter.html</anchorfile>
      <anchor>ade07dc52c22dac464cee977ffabb9eb6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TrackDataZeroMQIncomingAdapterTest</name>
    <filename>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</filename>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>ab462310811b3ae7956eb07a6d913296c</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>abb922f6e7e16a444a019242911ee96cd</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>createAdapter</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>abf1a6867ababb589068ae1a457c0c0f4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>TrackData</type>
      <name>createTestTrackData</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>a892a31fa0c2179897dbc1c4e8b7c1336</anchor>
      <arglist>(int32_t trackId)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; MockMessageSocket &gt;</type>
      <name>mockSocket_</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>a8c2a9e6491d2f107d1a10e11daa48621</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>MockMessageSocket *</type>
      <name>mockSocketPtr_</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>a27282a1c554e77aed8263db59f68e358</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::shared_ptr&lt; MockIncomingPort &gt;</type>
      <name>mockPort_</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>a9528a136ca70237d6012924d9a638d8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; TrackDataZeroMQIncomingAdapter &gt;</type>
      <name>adapter_</name>
      <anchorfile>d1/d03/classTrackDataZeroMQIncomingAdapterTest.html</anchorfile>
      <anchor>a7b8026400c8059aabce6061793b92997</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>adapters::common::messaging::ZeroMQSocket</name>
    <filename>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</filename>
    <base>adapters::common::messaging::IMessageSocket</base>
    <member kind="enumeration">
      <type></type>
      <name>SocketType</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a503cf095325bb589f8e2d899eeedc9ce</anchor>
      <arglist></arglist>
      <enumvalue file="db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html" anchor="a503cf095325bb589f8e2d899eeedc9cea241dd841abade20fcb27b8a9f494e1eb">SUB</enumvalue>
      <enumvalue file="db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html" anchor="a503cf095325bb589f8e2d899eeedc9cea9aba7127268ef2f384fdc95498c7bb1a">PUB</enumvalue>
      <enumvalue file="db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html" anchor="a503cf095325bb589f8e2d899eeedc9ceaf712f064807c1b144fbe7d0a950d9f07">DISH</enumvalue>
      <enumvalue file="db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html" anchor="a503cf095325bb589f8e2d899eeedc9ceaf46d45b52139dc3fa782c53f701ada0d">RADIO</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ConnectionMode</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a171ab154f16d21756dcb5b9abde3b791</anchor>
      <arglist></arglist>
      <enumvalue file="db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html" anchor="a171ab154f16d21756dcb5b9abde3b791a49ab28040dfa07f53544970c6d147e1e">Connect</enumvalue>
      <enumvalue file="db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html" anchor="a171ab154f16d21756dcb5b9abde3b791a0b172674284d79079624051d2ae11581">Bind</enumvalue>
    </member>
    <member kind="function">
      <type></type>
      <name>ZeroMQSocket</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a4013f04d1ebd67d7d0048cb01a0ce950</anchor>
      <arglist>(SocketType socketType, int ioThreads=1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ZeroMQSocket</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>afd9575854897b81b58ffa62476dd68ea</anchor>
      <arglist>(int socketType, ConnectionMode mode=ConnectionMode::Connect, int ioThreads=1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ZeroMQSocket</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a6e40b3c8d2e8bcdc553c6c9374aafd42</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>connect</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>aea26badae0f421560d7191aec8488388</anchor>
      <arglist>(const std::string &amp;endpoint) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>send</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a3d210a98aa0e70180fec8b69a7c0e5aa</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>send</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>af3e09ab5b3ef8b613b7fdf634e03186a</anchor>
      <arglist>(const std::vector&lt; uint8_t &gt; &amp;data, const std::string &amp;group) override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; std::vector&lt; uint8_t &gt; &gt;</type>
      <name>receive</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a8584e9a1393d72c3a91b87daf25226d8</anchor>
      <arglist>(int32_t timeoutMs) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a1c08247330e71bff4ee85b949ada7b7f</anchor>
      <arglist>() noexcept override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>abc715db00b3dc255eea97a8769099d6c</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getSocketType</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a981156caff9cdc168ae5715795b2cf73</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>connect</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>aea9d10f083cbe31c1d7d716bbb8f6bf3</anchor>
      <arglist>(const std::string &amp;endpoint, ConnectionMode mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>subscribe</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a558c9f5ca0a5e419a2ea8a769a6a7797</anchor>
      <arglist>(const std::string &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>joinGroup</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a2f355e65168b4b37e273cea49bd02a26</anchor>
      <arglist>(const std::string &amp;group)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ZeroMQSocket</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>aabf2d4112e65ceca9bec709c6a6d7a50</anchor>
      <arglist>(const ZeroMQSocket &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>ZeroMQSocket &amp;</type>
      <name>operator=</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>ae94110e003a50d745ae41bd419812fea</anchor>
      <arglist>(const ZeroMQSocket &amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ZeroMQSocket</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a503a6b6d91f6454fcf90adfff2f51144</anchor>
      <arglist>(ZeroMQSocket &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function">
      <type>ZeroMQSocket &amp;</type>
      <name>operator=</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a0f977b546729083dfe16a516fcaf254c</anchor>
      <arglist>(ZeroMQSocket &amp;&amp;)=delete</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>getSocketTypeName</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>ae6df858f5c7201d47418ab8470c41187</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; zmq::context_t &gt;</type>
      <name>context_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a4fe140aa4e82741db0f9c665abf5c7ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::unique_ptr&lt; zmq::socket_t &gt;</type>
      <name>socket_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a6e761efc773ad65863c135d2037961d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>socketType_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a6c6366a24df8f7a02248f92319d0cdb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ConnectionMode</type>
      <name>mode_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>aa2e7de350196ab7b21fad3f4e1c531ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::atomic&lt; bool &gt;</type>
      <name>connected_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>a63f56214cd69c6115643a0bdd83a1721</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>endpoint_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>adec3e7535988b74c9af18ed9fe405dbc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::mutex</type>
      <name>mutex_</name>
      <anchorfile>db/d01/classadapters_1_1common_1_1messaging_1_1ZeroMQSocket.html</anchorfile>
      <anchor>adbd633f0de75e26e84879ddbfb5fa901</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>a_hexagon</name>
    <filename>db/d00/namespacea__hexagon.html</filename>
    <namespace>a_hexagon::test</namespace>
  </compound>
  <compound kind="namespace">
    <name>a_hexagon::test</name>
    <filename>dd/d00/namespacea__hexagon_1_1test.html</filename>
    <namespace>a_hexagon::test::mocks</namespace>
  </compound>
  <compound kind="namespace">
    <name>a_hexagon::test::mocks</name>
    <filename>d4/d03/namespacea__hexagon_1_1test_1_1mocks.html</filename>
    <class kind="class">a_hexagon::test::mocks::MockIncomingPort</class>
    <class kind="class">a_hexagon::test::mocks::MockMessageSocket</class>
    <class kind="class">a_hexagon::test::mocks::MockOutgoingPort</class>
  </compound>
  <compound kind="namespace">
    <name>adapters</name>
    <filename>d2/d03/namespaceadapters.html</filename>
    <namespace>adapters::common</namespace>
    <namespace>adapters::incoming</namespace>
    <namespace>adapters::outgoing</namespace>
    <class kind="class">adapters::AdapterManager</class>
    <class kind="class">adapters::IAdapter</class>
    <class kind="class">adapters::MessagePipeline</class>
  </compound>
  <compound kind="namespace">
    <name>adapters::common</name>
    <filename>db/d03/namespaceadapters_1_1common.html</filename>
    <namespace>adapters::common::messaging</namespace>
  </compound>
  <compound kind="namespace">
    <name>adapters::common::messaging</name>
    <filename>d0/d00/namespaceadapters_1_1common_1_1messaging.html</filename>
    <namespace>adapters::common::messaging::anonymous_namespace{ZeroMQSocket.cpp}</namespace>
    <class kind="class">adapters::common::messaging::IMessageSocket</class>
    <class kind="class">adapters::common::messaging::ZeroMQSocket</class>
  </compound>
  <compound kind="namespace">
    <name>adapters::common::messaging::anonymous_namespace{ZeroMQSocket.cpp}</name>
    <filename>dc/d01/namespaceadapters_1_1common_1_1messaging_1_1anonymous__namespace_02ZeroMQSocket_8cpp_03.html</filename>
    <member kind="function">
      <type>int</type>
      <name>socketTypeEnumToZmq</name>
      <anchorfile>dc/d01/namespaceadapters_1_1common_1_1messaging_1_1anonymous__namespace_02ZeroMQSocket_8cpp_03.html</anchorfile>
      <anchor>a6a2e10d8cbdc50d43d4b406b1607d8b2</anchor>
      <arglist>(ZeroMQSocket::SocketType type)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>adapters::incoming</name>
    <filename>d4/d03/namespaceadapters_1_1incoming.html</filename>
    <namespace>adapters::incoming::zeromq</namespace>
  </compound>
  <compound kind="namespace">
    <name>adapters::incoming::zeromq</name>
    <filename>dd/d03/namespaceadapters_1_1incoming_1_1zeromq.html</filename>
    <class kind="class">adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter</class>
  </compound>
  <compound kind="namespace">
    <name>adapters::outgoing</name>
    <filename>df/d00/namespaceadapters_1_1outgoing.html</filename>
    <namespace>adapters::outgoing::zeromq</namespace>
  </compound>
  <compound kind="namespace">
    <name>adapters::outgoing::zeromq</name>
    <filename>dc/d00/namespaceadapters_1_1outgoing_1_1zeromq.html</filename>
    <class kind="class">adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter</class>
  </compound>
  <compound kind="namespace">
    <name>config</name>
    <filename>d6/d03/namespaceconfig.html</filename>
    <member kind="variable" static="yes">
      <type>static constexpr const char *</type>
      <name>TRACK_DATA_INCOMING_ENDPOINT</name>
      <anchorfile>d6/d03/namespaceconfig.html</anchorfile>
      <anchor>a4ba880f9e451e64a5101dcab10e14755</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr const char *</type>
      <name>TRACK_DATA_INCOMING_GROUP</name>
      <anchorfile>d6/d03/namespaceconfig.html</anchorfile>
      <anchor>ab959b5f176b4c2862fb215ed82b176f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr const char *</type>
      <name>EXTRAP_DATA_OUTGOING_ENDPOINT</name>
      <anchorfile>d6/d03/namespaceconfig.html</anchorfile>
      <anchor>a4909b2faabc3d31bb2d3a19e6945c301</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr const char *</type>
      <name>EXTRAP_DATA_OUTGOING_GROUP</name>
      <anchorfile>d6/d03/namespaceconfig.html</anchorfile>
      <anchor>afcdc4a84cdf75893324316f724f4885d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>domain</name>
    <filename>db/d03/namespacedomain.html</filename>
    <namespace>domain::adapters</namespace>
    <namespace>domain::logic</namespace>
    <namespace>domain::model</namespace>
    <namespace>domain::ports</namespace>
  </compound>
  <compound kind="namespace">
    <name>domain::adapters</name>
    <filename>d8/d03/namespacedomain_1_1adapters.html</filename>
    <namespace>domain::adapters::outgoing</namespace>
  </compound>
  <compound kind="namespace">
    <name>domain::adapters::outgoing</name>
    <filename>d2/d02/namespacedomain_1_1adapters_1_1outgoing.html</filename>
    <member kind="typedef" protection="private">
      <type>::adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter</type>
      <name>ExtrapTrackDataZeroMQOutgoingAdapter</name>
      <anchorfile>d2/d02/namespacedomain_1_1adapters_1_1outgoing.html</anchorfile>
      <anchor>a0d9df0457303db24bf7784112f7d502f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>domain::logic</name>
    <filename>da/d03/namespacedomain_1_1logic.html</filename>
    <class kind="class">domain::logic::TrackDataExtrapolator</class>
  </compound>
  <compound kind="namespace">
    <name>domain::model</name>
    <filename>dc/d01/namespacedomain_1_1model.html</filename>
    <class kind="class">domain::model::ExtrapTrackData</class>
    <class kind="class">domain::model::TrackData</class>
  </compound>
  <compound kind="namespace">
    <name>domain::ports</name>
    <filename>d5/d03/namespacedomain_1_1ports.html</filename>
    <namespace>domain::ports::incoming</namespace>
    <namespace>domain::ports::outgoing</namespace>
  </compound>
  <compound kind="namespace">
    <name>domain::ports::incoming</name>
    <filename>d7/d02/namespacedomain_1_1ports_1_1incoming.html</filename>
    <class kind="class">domain::ports::incoming::ITrackDataIncomingPort</class>
  </compound>
  <compound kind="namespace">
    <name>domain::ports::outgoing</name>
    <filename>de/d03/namespacedomain_1_1ports_1_1outgoing.html</filename>
    <class kind="class">domain::ports::outgoing::IExtrapTrackDataOutgoingPort</class>
  </compound>
  <compound kind="namespace">
    <name>utils</name>
    <filename>d6/d00/namespaceutils.html</filename>
    <class kind="class">utils::Logger</class>
  </compound>
  <compound kind="page">
    <name>md__2workspaces_2hexagon__c_2test_2a__hexagon_2test_2a__hexagon_2README</name>
    <title>a_hexagon Unit Tests</title>
    <filename>d4/d00/md__2workspaces_2hexagon__c_2test_2a__hexagon_2test_2a__hexagon_2README.html</filename>
  </compound>
  <compound kind="page">
    <name>md__2workspaces_2hexagon__c_2test_2a__hexagon_2README</name>
    <title>a_hexagon - Hexagonal Architecture Track Data Processing</title>
    <filename>d4/d00/md__2workspaces_2hexagon__c_2test_2a__hexagon_2README.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>a_hexagon - Track Data Extrapolation System</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="a_hexagon - Track Data Extrapolation System">mainpage</docanchor>
  </compound>
</tagfile>
