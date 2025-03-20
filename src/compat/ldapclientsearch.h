#pragma once

#include <KPim5/KLDAP/kldap/ldapclientsearch.h>

// Compatibility header for KLDAPCore/LdapClientSearch
namespace KLDAPCore {
using LdapClientSearch = KLDAP::LdapClientSearch;
}