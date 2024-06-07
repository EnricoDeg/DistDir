#
# @file example_basic1.jl
#
# @copyright Copyright (C) 2024 Enrico Degregori <enrico.degregori@gmail.com>
#
# @author Enrico Degregori <enrico.degregori@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# Note: export DL_LOAD_PATH=/path/to/DistDir/lib (not needed if LD_LIBRARY_PATH is set)
# Note: export JULIA_LOAD_PATH="/path/to/DistDir.jl:$JULIA_LOAD_PATH"

using DistDir
import MPI

DistDir.initialize()

DistDir.set_config_exchanger(DistDir.IsendIrecv2)
DistDir.set_config_verbose(DistDir.verbose_true)

size :: Int32 = 10

work_comm = MPI.COMM_WORLD
rank = MPI.Comm_rank(work_comm)
print("Hello world, I am rank $(MPI.Comm_rank(work_comm)) of $(MPI.Comm_size(work_comm))\n")
i :: Int = 0
if rank < 2
	id = 0
else
	id = 1
end

new_comm = MPI.COMM_NULL
DistDir.new_group(new_comm, work_comm, id)
print("Hello world, I am rank $(MPI.Comm_rank(new_comm)) of $(MPI.Comm_size(new_comm))\n")

list = Vector{Int32}([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
idxlist::DistDir.t_idxlist_jl = DistDir.new_idxlist(list)
idxlist_empty::DistDir.t_idxlist_jl = DistDir.new_idxlist()

a = Vector{Float64}(undef, size)
for i in eachindex(a)
	a[i] = 2 * i * (1-rank)
end

if rank < 1
	map :: DistDir.t_map_jl = DistDir.new_map(idxlist, idxlist_empty, work_comm)
else
	map :: DistDir.t_map_jl = DistDir.new_map(idxlist_empty, idxlist, work_comm)
end

exchanger :: DistDir.t_exchanger_jl = DistDir.new_exchanger(map, MPI.Datatype(Float64))

DistDir.exchanger_go(exchanger, a, a)

println(a)

DistDir.delete_exchanger(exchanger)
DistDir.delete_map(map)
DistDir.delete_idxlist(idxlist_empty)
DistDir.delete_idxlist(idxlist)

DistDir.finalize()