#!/usr/bin/env python

import pkglib

class PkgDb(object):
	def __init__(self, remotedb=False):
                self.__db = pkglib.db_open(remotedb)

        def close(self):
                pkglib.db_close(self.__db)

        def query(self, pkgname):
                return PkgIt(self.__db, pkgname)

class PkgIt(object):
        def __init__(self, db, pkgname):
                self.__it = pkglib.db_query(db, pkgname)

        def __iter__(self):
                return self

        def next(self):
                result = pkglib.db_list(self.__it)

                if not result:
                        raise StopIteration
                else:
                        return result


