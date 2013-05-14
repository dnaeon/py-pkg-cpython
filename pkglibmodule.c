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

