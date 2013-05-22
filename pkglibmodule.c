/*-
 * Copyright (c) 2013 Marin Atanasov Nikolov <dnaeon@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>

#include <pkg.h>

#include <Python.h>

void initpkglib(void);

static PyObject *pkglib_db_open(PyObject *self, PyObject *args);
static PyObject *pkglib_db_close(PyObject *self, PyObject *args);
static PyObject *pkglib_db_query_info(PyObject *self, PyObject *args);
static PyObject *pkglib_db_query_iter(PyObject *self, PyObject *args);
static PyObject *pkglib_db_query_install(PyObject *self, PyObject *args);
static PyObject *pkglib_db_query_install_iter(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_name(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_version(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_comment(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_origin(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_desc(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_mesg(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_arch(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_maint(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_www(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_prefix(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_infos(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_repopath(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_cksum(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_reponame(PyObject *self, PyObject *args);
static PyObject *pkglib_pkg_get_repourl(PyObject *self, PyObject *args);
static PyObject *pkglib_jobs_count(PyObject *self, PyObject *args);
static PyObject *pkglib_jobs_apply(PyObject *self, PyObject *args);

static PyObject *_pkglib_jobs_prep(PyObject *self, PyObject *args, pkg_flags f, pkg_jobs_t t);

static PyMethodDef
PkgLibMethods[] = {
	{ "db_open",           pkglib_db_open,           METH_VARARGS, NULL },
	{ "db_close",          pkglib_db_close,          METH_VARARGS, NULL },
	{ "db_query_info",     pkglib_db_query_info,     METH_VARARGS, NULL },
	{ "db_query_iter",     pkglib_db_query_iter,     METH_VARARGS, NULL },
	{ "db_query_install",  pkglib_db_query_install,  METH_VARARGS, NULL },
	{ "db_query_install_iter", pkglib_db_query_install_iter, METH_VARARGS, NULL },
	{ "pkg_get_name",      pkglib_pkg_get_name,      METH_VARARGS, NULL },
	{ "pkg_get_version",   pkglib_pkg_get_version,   METH_VARARGS, NULL },
	{ "pkg_get_comment",   pkglib_pkg_get_comment,   METH_VARARGS, NULL },
	{ "pkg_get_origin",    pkglib_pkg_get_origin,    METH_VARARGS, NULL },
	{ "pkg_get_desc",      pkglib_pkg_get_desc,      METH_VARARGS, NULL },
	{ "pkg_get_mesg",      pkglib_pkg_get_mesg,      METH_VARARGS, NULL },
	{ "pkg_get_arch",      pkglib_pkg_get_arch,      METH_VARARGS, NULL },
	{ "pkg_get_maint",     pkglib_pkg_get_maint,     METH_VARARGS, NULL },
	{ "pkg_get_www",       pkglib_pkg_get_www,       METH_VARARGS, NULL },
	{ "pkg_get_prefix",    pkglib_pkg_get_prefix,    METH_VARARGS, NULL },
	{ "pkg_get_infos",     pkglib_pkg_get_infos,     METH_VARARGS, NULL },
	{ "pkg_get_repopath",  pkglib_pkg_get_repopath,  METH_VARARGS, NULL },
	{ "pkg_get_cksum",     pkglib_pkg_get_cksum,     METH_VARARGS, NULL },
	{ "pkg_get_reponame",  pkglib_pkg_get_reponame,  METH_VARARGS, NULL },
	{ "pkg_get_repourl",   pkglib_pkg_get_repourl,   METH_VARARGS, NULL },
	{ "jobs_count",        pkglib_jobs_count,        METH_VARARGS, NULL },
	{ "jobs_apply",        pkglib_jobs_apply,        METH_VARARGS, NULL },
	{ NULL,                NULL,                     0,            NULL }, /* Sentinel */
};

static PyObject *
pkglib_db_open(PyObject *self, PyObject *args)
{
	struct pkgdb *db = NULL;
	PyObject *result = NULL;
	pkgdb_t db_type;
  
	pkg_init(NULL);

	if (PyArg_ParseTuple(args, "i", &db_type) == 0)
		return (NULL);

	if (pkgdb_open(&db, db_type) != EPKG_OK) {
		PyErr_SetString(PyExc_IOError, "Cannot open the package database");
		return (NULL);
	}

	result = PyCapsule_New(db, "pkglib.db", NULL);

	return (result);
}

static PyObject *
pkglib_db_close(PyObject *self, PyObject *args)
{
        struct pkgdb *db = NULL;
	PyObject *db_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &db_capsule) == 0)
		return (NULL);

	db = (struct pkgdb *)PyCapsule_GetPointer(db_capsule, "pkglib.db");

	pkgdb_close(db);

	Py_INCREF(Py_None);

	return (Py_None);
}

static PyObject *
pkglib_db_query_info(PyObject *self, PyObject *args)
{
	struct pkgdb *db = NULL;
	struct pkgdb_it *it = NULL;
	PyObject *db_capsule = NULL;
	PyObject *result = NULL;
	const char *pkgname = NULL;
	match_t match = MATCH_EXACT;
	bool match_regex = false;

	if (PyArg_ParseTuple(args, "Ozi", &db_capsule, &pkgname, &match_regex) == 0)
		return (NULL);

	if (match_regex)
		match = MATCH_REGEX;

	if (pkgname == NULL)
		match = MATCH_ALL;

	db = (struct pkgdb *)PyCapsule_GetPointer(db_capsule, "pkglib.db");

	if ((it = pkgdb_query(db, pkgname, match)) == NULL) {
		PyErr_SetString(PyExc_IOError, "Cannot query the package database");
		return (NULL);
	}

	result = PyCapsule_New(it, "pkglib.it", NULL);

	return (result);
}

static PyObject *
pkglib_db_query_install(PyObject *self, PyObject *args)
{
	pkg_flags f = PKG_FLAG_NONE | PKG_FLAG_PKG_VERSION_TEST;
	pkg_jobs_t t = PKG_JOBS_INSTALL;

	return (_pkglib_jobs_prep(self, args, f, t));
}

static PyObject *
_pkglib_jobs_prep(PyObject *self, PyObject *args, pkg_flags f, pkg_jobs_t t)
{
	struct pkgdb *db = NULL;
	struct pkg_jobs *jobs = NULL;
	PyObject *db_capsule = NULL,
		*result = NULL,
		*pkglist = NULL;
	char **pkgs;
	int pkgnum, retcode, i;
	match_t match = MATCH_EXACT;
	bool match_regex = false;
	
	if (PyArg_ParseTuple(args, "OOi", &db_capsule, &pkglist, &match_regex) == 0) 
		return (NULL);

	if (match_regex)
		match = MATCH_REGEX;

	/* Parse multiple packages passed as a list */
	if (PyList_Check(pkglist)) {
		pkgnum = PyList_Size(pkglist);
		
		if ((pkgs = calloc(pkgnum, sizeof(char *))) == NULL) {
			PyErr_SetString(PyExc_MemoryError, NULL);
			return (NULL);
		}

		for (i = 0; i < pkgnum; i++)
			pkgs[i] = PyString_AsString(PyList_GetItem(pkglist, i));

	} else if (PyString_Check(pkglist)) {
		/* Only a single package name provided as a string */
		pkgnum = 1;
		pkgs = calloc(pkgnum, sizeof(char *));
		pkgs[0] = PyString_AsString(pkglist);
	} else {
		PyErr_SetString(PyExc_TypeError, "Package name(s) should be a list or a string");
		return (NULL);
	}
	
	retcode = pkgdb_access(PKGDB_MODE_READ  |
			       PKGDB_MODE_WRITE |
			       PKGDB_MODE_CREATE,
			       PKGDB_DB_LOCAL   |
			       PKGDB_DB_REPO);

	if (retcode != EPKG_OK) {
		PyErr_SetString(PyExc_RuntimeError, "Insufficient privileges to install packages");
		return (NULL);
	}

	db = (struct pkgdb *)PyCapsule_GetPointer(db_capsule, "pkglib.db");

	if (pkg_jobs_new(&jobs, t, db) != EPKG_OK) {
		PyErr_SetString(PyExc_MemoryError, "Cannot create jobs object");
		return (NULL);
	}

	pkg_jobs_set_flags(jobs, f);

	if (pkg_jobs_add(jobs, match, pkgs, pkgnum) == EPKG_FATAL) {
		PyErr_SetString(PyExc_RuntimeError, "Cannot add job entries");
		return (NULL);
	}

	if (pkg_jobs_solve(jobs) != EPKG_OK) {
		PyErr_SetString(PyExc_RuntimeError, "Cannot solve jobs");
		return (NULL);
	}

	result = PyCapsule_New(jobs, "pkglib.jobs", NULL);

	return (result);
}

static PyObject *
pkglib_db_query_iter(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	struct pkgdb_it *it = NULL;
	PyObject *it_capsule = NULL;
	PyObject *result = NULL;

	if (PyArg_ParseTuple(args, "O", &it_capsule) == 0)
		return (NULL);

	it = (struct pkgdb_it *)PyCapsule_GetPointer(it_capsule, "pkglib.it");
	
	if (pkgdb_it_next(it, &pkg, PKG_LOAD_BASIC) == EPKG_OK)
		result = PyCapsule_New(pkg, "pkglib.pkg", NULL);
	else {
		pkgdb_it_reset(it);
		result = Py_None;
	}

	return (result);
}

static PyObject *
pkglib_jobs_count(PyObject *self, PyObject *args)
{
	struct pkg_jobs *jobs = NULL;
	PyObject *result = NULL;
	PyObject *jobs_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &jobs_capsule) == 0)
		return (NULL);

	jobs = (struct pkg_jobs *)PyCapsule_GetPointer(jobs_capsule, "pkglib.jobs");

	result = Py_BuildValue("i", pkg_jobs_count(jobs));

	return (result);
}

static PyObject *
pkglib_jobs_apply(PyObject *self, PyObject *args)
{
	struct pkg_jobs *jobs = NULL;
	PyObject *result = NULL;
	PyObject *jobs_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &jobs_capsule) == 0)
		return (NULL);

	jobs = (struct pkg_jobs *)PyCapsule_GetPointer(jobs_capsule, "pkglib.jobs");

	if (pkg_jobs_count(jobs) == 0) {
		Py_INCREF(Py_None);
		return (Py_None);
	}

	if (pkg_jobs_apply(jobs) != EPKG_OK) {
		PyErr_SetString(PyExc_RuntimeError, "Cannot apply package jobs");
		return (NULL);
	}

	Py_INCREF(Py_None);
	return (Py_None);
}

static PyObject *
pkglib_db_query_install_iter(PyObject *self, PyObject *args)
{
	static struct pkg *pkg = NULL;
	struct pkg_jobs *jobs = NULL;
	PyObject *result = NULL;
	PyObject *jobs_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &jobs_capsule) == 0)
		return (NULL);

	jobs = (struct pkg_jobs *)PyCapsule_GetPointer(jobs_capsule, "pkglib.jobs");

	if (pkg_jobs(jobs, &pkg) == EPKG_OK)
		result = PyCapsule_New(pkg, "pkglib.pkg", NULL);
	else
		result = Py_None;

	return (result);
}
static PyObject *
pkglib_pkg_get_name(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *name = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_NAME, &name);

	result = (PyObject *)Py_BuildValue("s", name);

	return (result);
}

static PyObject *
pkglib_pkg_get_version(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *version = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_VERSION, &version);

	result = (PyObject *)Py_BuildValue("s", version);

	return (result);
}

static PyObject *
pkglib_pkg_get_comment(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *comment = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_COMMENT, &comment);

	result = (PyObject *)Py_BuildValue("s", comment);

	return (result);
}

static PyObject *
pkglib_pkg_get_origin(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *origin = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_ORIGIN, &origin);

	result = (PyObject *)Py_BuildValue("s", origin);

	return (result);
}

static PyObject *
pkglib_pkg_get_desc(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *desc = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_DESC, &desc);

	result = (PyObject *)Py_BuildValue("s", desc);

	return (result);
}

static PyObject *
pkglib_pkg_get_mesg(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *mesg = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_MESSAGE, &mesg);

	result = (PyObject *)Py_BuildValue("s", mesg);

	return (result);
}						\

static PyObject *
pkglib_pkg_get_arch(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *arch = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_ARCH, &arch);

	result = (PyObject *)Py_BuildValue("s", arch);

	return (result);
}

static PyObject *
pkglib_pkg_get_maint(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *maint = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_MAINTAINER, &maint);

	result = (PyObject *)Py_BuildValue("s", maint);

	return (result);
}

static PyObject *
pkglib_pkg_get_www(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *www = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_WWW, &www);

	result = (PyObject *)Py_BuildValue("s", www);

	return (result);
}

static PyObject *
pkglib_pkg_get_prefix(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *prefix = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_PREFIX, &prefix);

	result = (PyObject *)Py_BuildValue("s", prefix);

	return (result);
}

static PyObject *
pkglib_pkg_get_infos(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *infos = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_INFOS, &infos);

	result = (PyObject *)Py_BuildValue("s", infos);

	return (result);
}

static PyObject *
pkglib_pkg_get_repopath(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *repopath = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_REPOPATH, &repopath);

	result = (PyObject *)Py_BuildValue("s", repopath);

	return (result);
}

static PyObject *
pkglib_pkg_get_cksum(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *cksum = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_CKSUM, &cksum);

	result = (PyObject *)Py_BuildValue("s", cksum);

	return (result);
}

static PyObject *
pkglib_pkg_get_reponame(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *reponame = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_REPONAME, &reponame);

	result = (PyObject *)Py_BuildValue("s", reponame);

	return (result);
}

static PyObject *
pkglib_pkg_get_repourl(PyObject *self, PyObject *args)
{
	struct pkg *pkg = NULL;
	const char *repourl = NULL;
	PyObject *result = NULL;
	PyObject *pkg_capsule = NULL;

	if (PyArg_ParseTuple(args, "O", &pkg_capsule) == 0)
		return (NULL);

	pkg = (struct pkg *)PyCapsule_GetPointer(pkg_capsule, "pkglib.pkg");

	pkg_get(pkg, PKG_REPOURL, &repourl);

	result = (PyObject *)Py_BuildValue("s", repourl);

	return (result);
}

PyMODINIT_FUNC
initpkglib(void)
{
	Py_InitModule("pkglib", PkgLibMethods);
}

