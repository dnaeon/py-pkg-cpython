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

#include <pkg.h>

#include <Python.h>

void initpkglib(void);

static PyObject *pkglib_db_open(PyObject *self, PyObject *args);
static PyObject *pkglib_db_close(PyObject *self, PyObject *args);
static PyObject *pkglib_db_query(PyObject *self, PyObject *args);
static PyObject *pkglib_db_list(PyObject *self, PyObject *args);

static PyMethodDef
PkgLibMethods[] = {
	{ "db_open",   pkglib_db_open,  METH_VARARGS, NULL },
	{ "db_close",  pkglib_db_close, METH_VARARGS, NULL },
	{ "db_query",  pkglib_db_query, METH_VARARGS, NULL },
	{ "db_list",   pkglib_db_list,  METH_VARARGS, NULL },
	{ NULL,        NULL,            0,            NULL }, /* Sentinel */
};

static PyObject *
pkglib_db_open(PyObject *self, PyObject *args)
{
	struct pkgdb *db = NULL;
	PyObject *result = NULL;
	pkgdb_t db_type;
  
	pkg_init(NULL);

	if (PyArg_ParseTuple(args, "i", &db_type) == 0) { 
		return (NULL);
	}

	if (pkgdb_open(&db, db_type) != EPKG_OK) {
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

	if (PyArg_ParseTuple(args, "O", &db_capsule) == 0) {
		return (NULL);
	}

	db = (struct pkgdb *)PyCapsule_GetPointer(db_capsule, "pkglib.db");

	pkgdb_close(db);

	Py_INCREF(Py_None);

	return (Py_None);
}

static PyObject *
pkglib_db_query(PyObject *self, PyObject *args)
{
	struct pkgdb *db = NULL;
	struct pkgdb_it *it = NULL;
	PyObject *db_capsule = NULL;
	PyObject *result = NULL;
	const char *pkgname = NULL;

	if (PyArg_ParseTuple(args, "Os", &db_capsule, &pkgname) == 0) {
		return (NULL);
	}

	db = (struct pkgdb *)PyCapsule_GetPointer(db_capsule, "pkglib.db");

	if ((it = pkgdb_query(db, pkgname, MATCH_GLOB)) == NULL) {
		return (NULL);
	}

	result = PyCapsule_New(it, "pkglib.it", NULL);

	return (result);
}

static PyObject *
pkglib_db_list(PyObject *self, PyObject *args)
{
	static struct pkg *pkg = NULL;
	struct pkgdb_it *it = NULL;
	const char *name,
		   *version,
		   *comment;
	PyObject *it_capsule = NULL;
	PyObject *result = NULL;

	if (PyArg_ParseTuple(args, "O", &it_capsule) == 0) {
		return (NULL);
	}

	it = (struct pkgdb_it *)PyCapsule_GetPointer(it_capsule, "pkglib.it");
	
	if (pkgdb_it_next(it, &pkg, PKG_LOAD_BASIC) == EPKG_OK) {
		pkg_get(pkg, PKG_NAME, &name, PKG_VERSION, &version, PKG_COMMENT, &comment);
		result = (PyObject *)Py_BuildValue("sss", name, version, comment);
	} else {
		result = Py_None;
	}

	return (result);
}

PyMODINIT_FUNC
initpkglib(void)
{
	Py_InitModule("pkglib", PkgLibMethods);
}

