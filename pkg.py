import pkglib

class PkgDb(object):
	def __init__(self, remotedb=False):
                self._db = pkglib.db_open(remotedb)

        def __del__(self):
                #pkglib.db_close(self._db)
                pass

        def close(self):
                return pkglib.db_close(self._db)

        def query_info(self, pkgname=None, match_regex=False):
                i = pkglib.db_query_info(self._db, pkgname, match_regex)
                return PkgIter(i)

        def query_install(self, pkgname, match_regex=False):
                j = pkglib.db_query_install(self._db, pkgname, match_regex)
                return PkgJobs(j)

        def query_delete(self, pkgname, match_regex=False):
                j = pkglib.db_query_delete(self._db, pkgname, match_regex)
                return PkgJobs(j)

        def query_autoremove(self):
                j = pkglib.db_query_autoremove(self._db)
                return PkgJobs(j)

        def query_upgrade(self):
                j = pkglib.db_query_upgrade(self._db)
                return PkgJobs(j)

class PkgIter(object):
        _load_flags = {
                'PKG_LOAD_BASIC'           : 0,
                'PKG_LOAD_DEPS'            : (1 << 0),
                'PKG_LOAD_RDEPS'           : (1 << 1),
                'PKG_LOAD_FILES'           : (1 << 2),
                'PKG_LOAD_SCRIPTS'         : (1 << 3),
                'PKG_LOAD_OPTIONS'         : (1 << 4),
                'PKG_LOAD_MTREE'           : (1 << 5),
                'PKG_LOAD_DIRS'            : (1 << 6),
                'PKG_LOAD_CATEGORIES'      : (1 << 7),
                'PKG_LOAD_LICENSES'        : (1 << 8),
                'PKG_LOAD_USERS'           : (1 << 9),
                'PKG_LOAD_GROUPS'          : (1 << 10),
                'PKG_LOAD_SHLIBS_REQUIRED' : (1 << 11),
                'PKG_LOAD_SHLIBS_PROVIDED' : (1 << 12),
                'PKG_LOAD_ANNOTATIONS'     : (1 << 13),
        }
                
        def __init__(self, it):
                self._it = it
                self._flags = self.__class__._load_flags['PKG_LOAD_BASIC']
        
        def __del__(self):
                #pkglib.db_query_iter_free(self._it)
                pass

        def __contains__(self, name):
                for p in self:
                        if p.name() == name or p.origin() == name:
                                return True

                return False

        def __len__(self):
                i = 0

                for p in self:
                        i += 1

                return i
                
        def __iter__(self):
                return self

        def next(self):
                result = pkglib.db_query_iter(self._it, self._flags)

                if not result:
                        raise StopIteration
                else:
                        return Pkg(result)

        def free(self):
                return pkglib.db_query_iter_free(self._it)

        def load_deps(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_DEPS']

        def load_rdeps(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_RDEPS']

        def load_files(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_FILES']

        def load_scripts(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_SCRIPTS']

        def load_options(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_OPTIONS']

        def load_mtree(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_MTREE']

        def load_dirs(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_DIRS']

        def load_categories(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_CATEGORIES']

        def load_licenses(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_LICENSES']

        def load_users(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_USERS']

        def load_groups(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_GROUPS']

        def load_shlibs_required(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_SHLIBS_REQUIRED']

        def load_shlibs_provided(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_SHLIBS_PROVIDED']

        def load_annotations(self):
                self._flags |= self.__class__._load_flags['PKG_LOAD_ANNOTATIONS']

class PkgJobs(object):
        def __init__(self, jobs):
                self._jobs = jobs

        def __del__(self):
                #pkglib.jobs_free(self._jobs)
                pass

        def __len__(self):
                return pkglib.jobs_count(self._jobs)

        def __contains__(self, name):
                for p in self:
                        if p.name() == name or p.origin() == name:
                                return True

                return False
                
        def __iter__(self):
                return self

        def next(self):
                result = pkglib.jobs_iter(self._jobs)

                if not result:
                        raise StopIteration

                return Pkg(result)

        def apply(self):
                return pkglib.jobs_apply(self._jobs)

        def free(self):
                return pkglib.jobs_free(self._jobs)
                        
class Pkg(object):
        def __init__(self, pkg):
                self._pkg = pkg

        def __del__(self):
                #pkglib.pkg_free(self._pkg)
                pass

        def __str__(self):
                return '%s-%s' % (self.name(), self.version())

        def __repr__(self):
                return self.origin()

        def free(self):
                return pkglib.pkg_free(self._pkg)

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

        def repourl(self):
                return pkglib.pkg_get_repourl(self._pkg)

        def deps(self):
                return PkgDepIter(self._pkg)

class PkgDepIter(object):
        def __init__(self, pkg):
                self._pkg = pkg

        def __del__(self):
                pass

        def __iter__(self):
                return self

        def __contains__(self, name):
                for d in self:
                        if d.name() == name or d.origin() == name:
                                return True

                return False

        def __len__(self):
                i = 0

                for d in self:
                        i += 1

                return i
                
        def next(self):
                result = pkglib.pkg_dep_iter(self._pkg)

                if not result:
                        raise StopIteration

                return PkgDep(result)

class PkgDep(object):
        def __init__(self, dep):
                self._dep = dep;

        def __del__(self):
                pass

        def __str__(self):
                return '%s-%s' % (self.name(), self.version())
                
        def name(self):
                return pkglib.pkg_dep_name(self._dep)

        def version(self):
                return pkglib.pkg_dep_version(self._dep)

        def origin(self):
                return pkglib.pkg_dep_origin(self._dep)


