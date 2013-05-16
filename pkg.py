import pkglib

class PkgDb(object):
	def __init__(self, remotedb=False):
                self._db = pkglib.db_open(remotedb)

        def close(self):
                pkglib.db_close(self._db)

        def query(self, pkgname, match_regex=False):
                return PkgIter(self._db, pkgname, match_regex)

class PkgIter(object):
        def __init__(self, db, pkgname, match_regex=False):
                self._it = pkglib.db_query_info(db, pkgname, match_regex)

        def __iter__(self):
                return self

        def next(self):
                result = pkglib.db_query_iter(self._it)

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

        def origin(self):
                return pkglib.pkg_get_origin(self._pkg)

        def desc(self):
                return pkglib.pkg_get_desc(self._pkg)

        def mesg(self):
                return pkglib.pkg_get_mesg(self._pkg)

