/*
 * SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QSet>
#include <QString>

namespace OCC::Mac::FileProviderDomainIdentifierPolicy
{

/**
 * @brief What to do about an account's file provider domain registration.
 */
enum class RegistrationAction {
    Abort, //!< Domain listing failed; register nothing and mint no identifier.
    Skip, //!< The account's stored identifier is already registered.
    AddStored, //!< Register the account's stored identifier again, rather than minting a new one.
    AddFresh, //!< The account has no stored identifier; mint one.
};

/**
 * @brief Decide how to register a file provider domain for an account.
 *
 * Minting a new identifier while the account already has one is what leaves a second
 * user-visible location behind: macOS keeps the previously registered domain, and the new
 * registration reuses its display name, so the system disambiguates the two by appending a
 * date to one of them. Re-adding an existing identifier, by contrast, replaces the
 * registration in place.
 *
 * @param storedIdentifier The account's persisted domain identifier, empty if it has none.
 * @param registeredIdentifiers Identifiers of the domains macOS currently reports.
 * @param domainListingSucceeded Whether those identifiers could be read at all. When false,
 *        the set of registered domains is unknown rather than empty, so the only safe answer
 *        is `Abort`.
 */
[[nodiscard]] RegistrationAction decideRegistration(const QString &storedIdentifier, const QSet<QString> &registeredIdentifiers, bool domainListingSucceeded);

} // namespace OCC::Mac::FileProviderDomainIdentifierPolicy
