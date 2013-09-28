#include <unistd.h>
#include "crtools.h"
#include "proc_parse.h"
#include "log.h"

static unsigned int cr_uid; /* UID which user can C/R */

/*
 * Setup what user is requesting for dump (via rpc or using 
 * suid bit on crtools). Later we would deny to dump/restore 
 * a task, to which the original user doesn't have the direct 
 * access to. (Or implement some trickier security policy).
 */

void restrict_uid(unsigned int uid)
{
	pr_info("Restrict C/R with %u uid\n", uid);
	cr_uid = uid;
}

static bool check_uid(unsigned int uid)
{
	if (cr_uid == 0)
		return true;
	if (cr_uid == uid)
		return true;

	return false;
}

bool may_dump(struct proc_status_creds *creds)
{
	unsigned int uid = creds->uids[0];

	if (check_uid(uid))
		return true;

	pr_err("UID (%u) != dumper's UID(%u)\n", uid, cr_uid);
	return false;
}

bool may_restore(CredsEntry *creds)
{
	unsigned int uid = creds->uid;

	if (check_uid(uid))
		return true;

	pr_err("UID (%u) != restorer's UID(%u)\n", uid, cr_uid);
	return false;
}