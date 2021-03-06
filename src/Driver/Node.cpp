#include "stdafx.h"
#include "Node.h"
//Hmm, we could probably wholesale replace Node with NodeDescriptor if node doesn't end up having any functionality..
Node::Node(const NodeDescriptor& desc)
	: m_id(desc.id)
	, m_name(desc.displayName)
	, m_type(desc.type)
	, m_support()
{
	if (desc.apiSupport & nsvr_api_supports_buffered) {
		m_support.insert(Apis::Buffered);
	}
	if (desc.apiSupport & nsvr_api_supports_waveform) {
		m_support.insert(Apis::Waveform);
	}
}

Node::Node()
	: m_id(0)
	, m_name()
	, m_type()
{

}


NodeId<local> Node::id() const
{
	return NodeId<local>{m_id};
}

std::string Node::name() const
{
	return m_name;
}

nsvr_node_concept Node::type() const
{
	return m_type;
}


