/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

© 2019 Triad National Security, LLC. All rights reserved.

This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL),
which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration.

All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration.
The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce,
prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef DSPEC50PARAMETERS_H
#define DSPEC50PARAMETERS_H

//DSPEC50Parameters.h

#define DPLEN 64

typedef struct _dspec_parameters
{
	char active_devices					[DPLEN];
	char blre_automatic_enable			[DPLEN];
	char correction_flat				[DPLEN];
	char correction_flat_list			[DPLEN];
	char date							[DPLEN];
	char date_start						[DPLEN];
	char device							[DPLEN];
	char gain_adjustment				[DPLEN];
	char gain_channel					[DPLEN];
	char gain_coarse					[DPLEN];
	char gain_coarse_list				[DPLEN];
	char gain_conversion				[DPLEN];
	char gain_conversion_list			[DPLEN];
	char gain_fine						[DPLEN];
	char gain_fine_list					[DPLEN];
	char gain_polarity					[DPLEN];
	char gain_stabilization_enable		[DPLEN];
	char gain_width						[DPLEN];
	char gate							[DPLEN];
	char hv								[DPLEN];
	char hv_enable						[DPLEN];
	char hv_list						[DPLEN];
	char hv_overload					[DPLEN];
	char hv_polarity					[DPLEN];
	char hv_volts						[DPLEN];
	char live							[DPLEN];
	char live_preset					[DPLEN];
	char lld							[DPLEN];
	char network_address				[DPLEN];
	char network_id						[DPLEN];
	char pz								[DPLEN];
	char pz_automatic					[DPLEN];
	char pz_enable						[DPLEN];
	char shap_cusp						[DPLEN];
	char shap_cusp_list					[DPLEN];
	char shap_flat						[DPLEN];
	char shap_flat_list					[DPLEN];
	char shap_rise						[DPLEN];
	char shap_rise_list					[DPLEN];
	char shutdown						[DPLEN];
	char shutdown_list					[DPLEN];
	char shutdown_type_selected			[DPLEN];
	char shut_actual					[DPLEN];
	char stabilization_mode				[DPLEN];
	char time							[DPLEN];
	char time_start						[DPLEN];
	char true_time						[DPLEN];
	char uld							[DPLEN];
	char version						[DPLEN];
	char zdt_enable						[DPLEN];

} DSPEC50_PARAMETERS;

#endif //DSPEC50PARAMETERS_H