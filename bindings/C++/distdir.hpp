/*
 * @file distdir.hpp
 *
 * @copyright Copyright (C) 2024 Enrico Degregori <enrico.degregori@gmail.com>
 *
 * @author Enrico Degregori <enrico.degregori@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.

 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef DISTDIR_HPP
#define DISTDIR_HPP

#include <vector>
#include <memory>
extern "C" {
	#include "src/distdir.h"
}

namespace distdir {

class distdir {

	public:

		typedef std::shared_ptr<distdir> Ptr;

		distdir() {
			distdir_initialize();
		}

		~distdir() {
			distdir_finalize();
		}

		void set_exchanger(int exchanger_type) {
			set_config_exchanger(exchanger_type);
		}

		void set_verbose(int verbose_type) {
			set_config_verbose(verbose_type);
		}

		int get_exchanger() {
			return get_config_exchanger();
		}

		int get_verbose() {
			return get_config_verbose();
		}

};

class idxlist {

	public:

		typedef std::shared_ptr<idxlist> Ptr;

		idxlist() {
			m_idxlist = new_idxlist_empty();
		}

		idxlist(std::vector<int>& list) {
			m_idxlist = new_idxlist(list.data(), list.size());
		}

		t_idxlist * get() {
			return m_idxlist;
		}

		~idxlist() {
			delete_idxlist(m_idxlist);
		}

	private:
		t_idxlist *m_idxlist;
};

class map {

	public:
		typedef std::shared_ptr<map> Ptr;

		map(idxlist::Ptr src_idxlist, idxlist::Ptr dst_idxlist, MPI_Comm comm) {
			m_map = new_map(src_idxlist->get(), dst_idxlist->get(), -1, comm);
		}

		map(idxlist::Ptr src_idxlist, idxlist::Ptr dst_idxlist, int stride, MPI_Comm comm) {
			m_map = new_map(src_idxlist->get(), dst_idxlist->get(), stride, comm);
		}

		map(map::Ptr map2d, int nlevels) {
			m_map = extend_map_3d(map2d->get(), nlevels);
		}

		t_map * get() {
			return m_map;
		}

		~map() {
			delete_map(m_map);
		}

	private:
		t_map *m_map;
};

template<class T>
class exchanger {

	public:
		typedef std::shared_ptr<exchanger<T>> Ptr;
		exchanger(map::Ptr map, MPI_Datatype type, distdir_hardware hw=CPU) {
			m_exchanger = new_exchanger(map->get(), type, hw);
		}

		void go(std::vector<T>& src_data, std::vector<T>& dst_data) {
			exchanger_go(m_exchanger, src_data.data(), dst_data.data());
		}

		~exchanger() {
			delete_exchanger(m_exchanger);
		}
	private:
		t_exchanger *m_exchanger;
};

}

#endif