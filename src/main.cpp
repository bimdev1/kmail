/*
 * kmail: KDE mail client
 * SPDX-FileCopyrightText: 1996-1998 Stefan Taferner <taferner@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include <kontactinterface/pimuniqueapplication.h>
#include "config-kmail.h"
#include "kmail_options.h"
#include "kmkernel.h" //control center

#include "kmail_debug.h"
#undef Status // stupid X headers

#include "aboutdata.h"

#include <KCrash>
#if !defined(Q_OS_WIN)
#include <KStartupInfo>
#endif
#include <KWindowSystem>
#include <QApplication>
#include <QDir>
#include <QSessionManager>
#include <QWebEngineUrlScheme>

#if KMAIL_WITH_KUSERFEEDBACK
#include <KUserFeedback/Provider>
#include "userfeedback/kmailuserfeedbackprovider.h"
#endif

#include <KIconTheme>

#include <KStyleManager>

//-----------------------------------------------------------------------------

class KMailApplication : public KontactInterface::PimUniqueApplication
{
public:
    KMailApplication(int& argc, char** argv[]) : KontactInterface::PimUniqueApplication(argc, argv) {}

    int activate(const QStringList& args, const QString& workingDir) override;
    void commitData(QSessionManager& sm);
    void setEventLoopReached();
    void delayedInstanceCreation(const QStringList& args, const QString& workingDir);

public Q_SLOTS:
    int newInstance(const QByteArray& startupId, const QStringList& arguments,
                    const QString& workingDirectory) override;

protected:
    bool mDelayedInstanceCreation = false;
    bool mEventLoopReached = false;
};

void KMailApplication::commitData(QSessionManager&)
{
    kmkernel->dumpDeadLetters();
    kmkernel->setShuttingDown(true); // Prevent further dumpDeadLetters calls
}

void KMailApplication::setEventLoopReached()
{
    mEventLoopReached = true;
}

int KMailApplication::newInstance(const QByteArray& startupId, const QStringList& arguments,
                                  const QString& workingDirectory)
{
#if !defined(Q_OS_WIN)
    if (KWindowSystem::isPlatformX11()) {
        KStartupInfo::setNewStartupId(kmkernel->mainWin()->windowHandle(), startupId);
    } else if (KWindowSystem::isPlatformWayland()) {
        KWindowSystem::setCurrentXdgActivationToken(QString::fromUtf8(startupId));
    }
#endif
    if (!kmkernel->firstInstance() && !arguments.isEmpty()) {
        // if we're going to create a new window (viewer or composer),
        // don't bring the mainwindow onto the current desktop
        return activate(arguments, workingDirectory);
    } else {
        return PimUniqueApplication::newInstance(startupId, arguments, workingDirectory);
    }
}

int KMailApplication::activate(const QStringList& args, const QString& workingDir)
{
    // If the event loop hasn't been reached yet, the kernel is probably not
    // fully initialized. Creating an instance would therefore fail, this is why
    // that is delayed until delayedInstanceCreation() is called.
    if (!mEventLoopReached) {
        qCDebug(KMAIL_LOG) << "Delaying instance creation.";
        mDelayedInstanceCreation = true;
        return 0;
    }

    if (!kmkernel) {
        return 0;
    }
    if (kmkernel->shuttingDown()) {
        qCDebug(KMAIL_LOG) << "KMail is in a shutdown mode.";
        return 0;
    }

    if (!kmkernel->firstInstance() || !qApp->isSessionRestored()) {
        kmkernel->handleCommandLine(true, args, workingDir);
    }
    kmkernel->setFirstInstance(false);

    KWindowSystem::activateWindow(kmkernel->mainWin()->windowHandle());

    return 0;
}

void KMailApplication::delayedInstanceCreation(const QStringList& args, const QString& workingDir)
{
    if (mDelayedInstanceCreation) {
        activate(args, workingDir);
    }
}

int main(int argc, char* argv[])
{
    KIconTheme::initTheme();
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    // Necessary for "cid" support in kmail.
    QWebEngineUrlScheme cidScheme("cid");
    cidScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored |
                       QWebEngineUrlScheme::LocalScheme | QWebEngineUrlScheme::LocalAccessAllowed);
    cidScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    QWebEngineUrlScheme::registerScheme(cidScheme);

    KMailApplication app(argc, &argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("kmail"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kmail")));
    app.setDesktopFileName(QStringLiteral("org.kde.kmail2"));

    KStyleManager::initStyle();

    KMail::AboutData about;
    app.setAboutData(about);
    KCrash::initialize();

    QCommandLineParser* cmdArgs = app.cmdArgs();
    kmail_options(cmdArgs);

    const QStringList args = QApplication::arguments();
    cmdArgs->process(args);
    about.processCommandLine(cmdArgs);

#if KMAIL_WITH_KUSERFEEDBACK
    if (cmdArgs->isSet(QStringLiteral("feedback"))) {
        KMailUserFeedbackProvider userFeedback(nullptr);
        QTextStream(stdout) << userFeedback.describeDataSources() << '\n';
        return 0;
    }
#endif

    if (!KMailApplication::start(args)) {
        qCDebug(KMAIL_LOG) << "Another instance of KMail already running";
        return 0;
    }
    // local, do the init
    KMKernel kmailKernel;
    kmailKernel.init();

    // and session management
    kmailKernel.doSessionManagement();

    // any dead letters?
    kmailKernel.recoverDeadLetters();

    kmkernel->setupDBus(); // Ok. We are ready for D-Bus requests.

    // If the instance hasn't been created yet, do that now
    app.setEventLoopReached();
    app.delayedInstanceCreation(args, QDir::currentPath());
    // kmkernel->setSystemTryAssociatedWindow(app.focusWindow());

    // Go!
    const int ret = qApp->exec();
    // clean up
    kmailKernel.cleanup();
    return ret;
}
