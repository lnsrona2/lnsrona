#include "decl_context.h"
#include "declaration.h"
#include <algorithm>
using namespace C1::AST;

DeclContext::InsertionResult DeclContext::add(Declaration* declaration)
{
	if (!m_pTableView && m_ListView.size() >= IndexizeThreshold)
		Indexize();

	m_ListView.push_back(declaration);
	if (m_pTableView)
	{
		auto pNameDecl = dynamic_cast<NamedDeclaration*>(declaration);
		m_pTableView->insert(std::make_pair(pNameDecl->Name(), pNameDecl));
	}
	return Success;
}

DeclContext::InsertionResult DeclContext::add(NamedDeclaration* declaration, NameCollisonPolicy name_policy)
{
	if (!m_pTableView && m_ListView.size() >= IndexizeThreshold)
		Indexize();

	InsertionResult result = Success;
	auto decl = lookup_local(declaration->Name());
	if (decl)
	{
		if (Declaration::CheckCompatible(decl, declaration))
			result = Success_CompatibleRedefinition;
		else
			result = Success_IncompatibleRedefinition;
	}
	if (result > name_policy)
		return InsertionResult(result + 2);

	m_ListView.push_back(declaration);
	if (m_pTableView)
		m_pTableView->insert(std::make_pair(declaration->Name(), declaration));
	declaration->SetAffiliation(this);
	return result;
}


void DeclContext::Indexize()
{
	if (m_pTableView) return;
	m_pTableView = std::make_unique<std::multimap<std::string, NamedDeclaration*>>();
	for (auto itr = m_ListView.rbegin(); itr != m_ListView.rend(); ++itr)
	{
		auto pNameDecl = dynamic_cast<NamedDeclaration*>(*itr);
		m_pTableView->insert(std::make_pair(pNameDecl->Name(), pNameDecl));
	}
}

const Declaration* DeclContext::lookup_local(const std::string& name) const
{
	if (m_pTableView)
	{
		auto itr = m_pTableView->find(name);
		if (itr != m_pTableView->end())
			return itr->second;
	}
	else
	{
		auto itr = std::find_if(m_ListView.rbegin(), m_ListView.rend(),
			[&name](const Declaration* decl)
		{
			auto name_decl = dynamic_cast<const NamedDeclaration*>(decl);
			return name_decl && name_decl->Name() == name;
		});
		return *itr;
	}
}

const Declaration* DeclContext::lookup(const std::string &name) const
{
	const DeclContext* pContext = this;
	while (pContext)
	{
		auto pDecl = pContext->lookup_local(name);
		if (pDecl)
			return pDecl;
		else
			pContext = pContext->parent();
	}
	return nullptr;
}