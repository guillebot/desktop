/*
 * SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "fileproviderdomainidentifierpolicy.h"

namespace OCC::Mac::FileProviderDomainIdentifierPolicy
{

RegistrationAction decideRegistration(const QString &storedIdentifier, const QSet<QString> &registeredIdentifiers, const bool domainListingSucceeded)
{
    if (!domainListingSucceeded) {
        return RegistrationAction::Abort;
    }

    if (storedIdentifier.isEmpty()) {
        return RegistrationAction::AddFresh;
    }

    if (registeredIdentifiers.contains(storedIdentifier)) {
        return RegistrationAction::Skip;
    }

    return RegistrationAction::AddStored;
}

} // namespace OCC::Mac::FileProviderDomainIdentifierPolicy
