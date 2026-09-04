/*
 * SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
 * SPDX-License-Identifier: CC0-1.0
 *
 * This software is in the public domain, furnished "as is", without technical
 * support, and with no warranty, express or implied, as to its usefulness for
 * any purpose.
 */

#include <QtTest>

#include "macOS/fileproviderdomainidentifierpolicy.h"

using namespace OCC::Mac::FileProviderDomainIdentifierPolicy;

/**
 * @brief Pins the choice between reusing an account's file provider domain identifier and
 * minting a new one.
 *
 * Minting a new identifier for an account that already has one is not a visibly broken
 * operation: the domain is added, the client keeps syncing, and the only symptom is a second
 * entry under ~/Library/CloudStorage whose display name macOS has disambiguated with a date.
 * The stale identifier stays behind in the extension's preferences, so nothing later in the
 * client notices either.
 *
 * The decision is kept free of FileProvider and of Objective-C so the cases that actually
 * caused this — a domain listing that failed, and a domain that is simply absent — can be
 * asserted without a registered domain or a running extension.
 */
class TestFileProviderDomainIdentifierPolicy : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    /// A failed listing says nothing about what is registered, so the stored identifier must
    /// not be replaced on the strength of it.
    void listingFailureDoesNotMintANewIdentifier()
    {
        const auto stored = QStringLiteral("0bd6be4e-6151-4db4-9668-57d8503d6d3f");
        QCOMPARE(decideRegistration(stored, {}, false), RegistrationAction::Abort);
    }

    /// Nor may a failed listing be read as "no domains yet" for an account without one.
    void listingFailureWithoutStoredIdentifierAborts()
    {
        QCOMPARE(decideRegistration({}, {}, false), RegistrationAction::Abort);
    }

    void storedIdentifierAlreadyRegisteredIsLeftAlone()
    {
        const auto stored = QStringLiteral("b375bcfe-1653-457b-ab49-fca678c8cd6d");
        QCOMPARE(decideRegistration(stored, {stored}, true), RegistrationAction::Skip);
    }

    /// The regression: the domain is gone but the account still knows its identifier, so the
    /// registration is replaced rather than duplicated.
    void vanishedStoredIdentifierIsRegisteredAgain()
    {
        const auto stored = QStringLiteral("0bd6be4e-6151-4db4-9668-57d8503d6d3f");
        const auto unrelated = QStringLiteral("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
        QCOMPARE(decideRegistration(stored, {unrelated}, true), RegistrationAction::AddStored);
    }

    /// Same when the system reports no domains at all, which is what a removal in System
    /// Settings leaves behind.
    void storedIdentifierSurvivesAnEmptyRegistration()
    {
        const auto stored = QStringLiteral("0bd6be4e-6151-4db4-9668-57d8503d6d3f");
        QCOMPARE(decideRegistration(stored, {}, true), RegistrationAction::AddStored);
    }

    void firstEnableMintsAFreshIdentifier()
    {
        QCOMPARE(decideRegistration({}, {}, true), RegistrationAction::AddFresh);
    }
};

QTEST_APPLESS_MAIN(TestFileProviderDomainIdentifierPolicy)
#include "testfileproviderdomainidentifierpolicy.moc"
