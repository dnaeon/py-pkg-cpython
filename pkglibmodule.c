#include <pkg.h>

#include <Python.h>

void initpkglib(void);

static PyObject *pkglib_db_open(PyObject *self, PyObject *args);
static PyObject *pkglib_db_close(PyObject *self, PyObject *args);

static PyMethodDef
PkgLibMethods[] = {
	{ "db_open",   pkglib_db_open, METH_VARARGS, NULL },
	{ "db_close",  pkglib_db_close, METH_VARARGS, NULL },
	{ NULL, NULL, NULL, NULL } /* Sentinel */
};

static PyObject *
pkglib_db_open(PyObject *self, PyObject *args)
{
	struct pkgdb *db;
	PyObject *result;
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

PyMODINIT_FUNC
initpkglib(void)
{
	Py_InitModule("pkglib", PkgLibMethods);
}

