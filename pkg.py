import pkglib

class PkgDb(object):
	def __init__(self, remotedb=False):
                self._db = pkglib.db_open(remotedb)

        def __del__(self):
                pass

        def close(self):
                pkglib.db_close(self._db)

        def query(self, pkgname=None, match_regex=False):
                return PkgIter(self._db, pkgname, match_regex)

class PkgIter(object):
        def __init__(self, db, pkgname, match_regex=False):
                self._it = pkglib.db_query_info(db, pkgname, match_regex)

        def __del__(self):
                pass

        def __contains__(self, name):
                for p in self:
                        if p.name() == name or p.origin() == name:
                                return True

                return False

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

        def __del__(self):
                pass

        def __str__(self):
                return '%s-%s: %s' % (self.name(), self.version(), self.comment())

        def __repr__(self):
                return self.origin()

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

        def arch(self):
                return pkglib.pkg_get_arch(self._pkg)

        def maint(self):
                return pkglib.pkg_get_maint(self._pkg)

        def www(self):
                return pkglib.pkg_get_www(self._pkg)

        def prefix(self):
                return pkglib.pkg_get_prefix(self._pkg)

        def infos(self):
                return pkglib.pkg_get_infos(self._pkg)

        def repopath(self):
                return pkglib.pkg_get_repopath(self._pkg)

        def cksum(self):
                return pkglib.pkg_get_cksum(self._pkg)

        def reponame(self):
                return pkglib.pkg_get_reponame(self._pkg)


