#pragma once
#include "hook.h"

struct IInterface
{
public:
    virtual ~IInterface() {};
};

class ITask
{
public:
    virtual bool AreYouDoneYet(void) THUNK;
};

class IUserCommand : IInterface {
public:
	virtual void ProcessCommand(const char *command, const char *parameters) THUNK;
};

class IUserCommandProxy {
public:
    virtual ~IUserCommandProxy() THUNK;

    virtual void Initialise(void) THUNK;
    virtual void RegisterCommand(IUserCommand *) THUNK;
    virtual void UnregisterCommand(IUserCommand *) THUNK;
    virtual void IssueCommand(const char *, IUserCommand *) THUNK;
};

class IUserCommandProxyHook : public AutoHook::Type<IUserCommandProxy> {
public:
    ~IUserCommandProxyHook() override {
#ifdef USERCMDPROXY_DEBUGGING
        LOG("[IUserCommandProxy::Release]");
#endif
        pBase->~IUserCommandProxy();
    };

    void Initialise(void) override {
#ifdef USERCMDPROXY_DEBUGGING
        LOG("[IUserCommandProxy::Initialise]");
#endif
        pBase->Initialise();
    };

    void RegisterCommand(IUserCommand *userCmd) override {
#ifdef USERCMDPROXY_DEBUGGING
        DWORD retAddr = *(DWORD*)(&userCmd - 1);
        LogFile::Format("[IUserCommandProxy::RegisterCommand] (return:0x%X)\n", retAddr);
#endif
        pBase->RegisterCommand(userCmd);
    };

    void UnregisterCommand(IUserCommand *userCmd) override {
#ifdef USERCMDPROXY_DEBUGGING
        DWORD retAddr = *(DWORD*)(&userCmd - 1);
        LogFile::Format("[IUserCommandProxy::UnegisterCommand] (return:0x%X)\n", retAddr);
#endif
        pBase->UnregisterCommand(userCmd);
    };

    void IssueCommand(const char *command, IUserCommand *userCmd) override {
#ifdef USERCMDPROXY_DEBUGGING
        if (strlen(command) > 0) {
            DWORD retAddr = *(DWORD*)(&command - 1);
            debugf("[IUserCommandProxy::IssueCommand] -> \"%s\", 0x%X (return:0x%X)\n", command, userCmd, retAddr);
        }
#endif
        pBase->IssueCommand(command, userCmd);
    };
};

IUserCommandProxy * AutoHook::Type<IUserCommandProxy>::pBase;