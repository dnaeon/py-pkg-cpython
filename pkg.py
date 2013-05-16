import pkglib

class PkgDb(object):
	def __init__(self, remotedb=False):
                self._db = pkglib.db_open(remotedb)

        def close(self):
                pkglib.db_close(self._db)

        def query(self, pkgname):
                return PkgIt(self._db, pkgname)

class PkgIt(PkgDb):
        def __init__(self, db, pkgname):
                self._it = pkglib.db_query(db, pkgname)

        def __iter__(self):
                return self

        def next(self):
                result = pkglib.db_list(self._it)

                if not result:
                        raise StopIteration
                else:
                        return Pkg(result)

class Pkg(object):
        def __init__(self, pkg):
                self._pkg = pkg

        def __str__(self):
                return '%s-%s: %s' % (self.name(), self.version(), self.comment())

        def name(self):
                return pkglib.pkg_get_name(self._pkg)

        def version(self):
                return pkglib.pkg_get_version(self._pkg)

        def comment(self):
                return pkglib.pkg_get_comment(self._pkg)


