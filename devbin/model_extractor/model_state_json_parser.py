#!/usr/bin/env python3
#
# Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
# or more contributor license agreements. Licensed under the Elastic License
# 2.0 and the following additional limitation. Functionality enabled by the
# files subject to the Elastic License 2.0 may only be used in production when
# invoked by an Elasticsearch process with a license key installed that permits
# use of machine learning features. You may not use this file except in
# compliance with the Elastic License 2.0 and the foregoing additional
# limitation.
#

#
# Description:
# Example python script demonstrating the steps required to parse a sequence
# of documents containing residual model state as generated by the model_extractor
# executable and printing statistics of interest.
#
# Requirements:
# * Python 3.x
# * The python 'sh' module - `pip install sh`. As such this is not portable to Windows.
# * The 'jq' utility version 1.6 from https://stedolan.github.io/jq/download/
#
# Usage:
# model_state_parser [<filename>|<stdin>]
# Input is expected to be a sequence of documents in the following format separated by a newline and a zero byte
# {"index":{"_id":"job_model_state_1560356306"}}
#{"residual_model":{"one-of-n":{"7.1":"","model":{"weight":{"log_weight":"-9.35028782102828","long_term_log_weight":"0"},"prior":{"gamma":{"decay_rate":"1.666667e-5","offset":"1e-1","likelihood_shape":"24.1388909650153","log_samples_mean":"449.571830398649:6.88716203006907","sample_moments":"449.571830398649:1000.22712712371:39163.4813871217","prior_shape":"1","prior_rate":"0","number_samples":"449.5318"}}},"model":{"weight":{"log_weight":"-14.3154880402178","long_term_log_weight":"0"},"prior":{"log_normal":{"decay_rate":"1.666667e-5","offset":"0","gaussian_mean":"6.888133","gaussian_precision":"447.3072","gamma_shape":"224.6533","gamma_rate":"9.74083674670471","number_samples":"447.3072"}}},"model":{"weight":{"log_weight":"0","long_term_log_weight":"0"},"prior":{"normal":{"decay_rate":"1.666667e-5","gaussian_mean":"1000.141","gaussian_precision":"447.3072","gamma_shape":"224.6533","gamma_rate":"8784551.35314265","number_samples":"447.3072"}}},"model":{"weight":{"log_weight":"-7858.45702705091","long_term_log_weight":"0"},"prior":{"poisson":{"decay_rate":"1.666667e-5","offset":"0","shape":"447192.9","rate":"447.3073","number_samples":"447.3072"}}},"model":{"weight":{"log_weight":"-1.79761523196536e308","long_term_log_weight":"0"},"prior":{"multimodal":{"clusterer":{"x_means_online_1d":{"cluster":{"index":"0","prior":{"decay_rate":"1.666667e-5","gaussian_mean":"1000.137","gaussian_precision":"447.4128","gamma_shape":"224.7061","gamma_rate":"8811514.81143192","number_samples":"447.4128"},"structure":{"decay_rate":"1.666667e-5","space":"12","category":{"size":"12","first":"10.17099:532.8519:4185.651","first":"22.8169:666.7236:845.4413","first":"33.18106:754.5582:544.9724","first":"58.12263:841.1187:698.8327","first":"77.89255:931.1263:699.1117","first":"64.03831:1010.521:430.7927","first":"57.25354:1080.089:403.1287","first":"54.14259:1155.511:555.933","first":"30.66816:1231.303:424.7728","first":"16.73802:1295.508:274.7627","first":"13.92944:1363.561:637.0926","first":"8.452876:1491.145:4246.771"},"points":"914;3.333334e-2:1142;3.333334e-2"}},"available_distributions":"7","decay_rate":"1.666667e-5","history_length":"37947.81","smallest":"110","largest":"1760","weight":"1","cluster_fraction":"5e-2","minimum_cluster_count":"12","winsorisation_confidence_interval":"1","index_generator":{"index":"1"}}},"seed_prior":{"one-of-n":{"7.1":"","model":{"weight":{"log_weight":"0","long_term_log_weight":"0"},"prior":{"gamma":{"decay_rate":"1.666667e-5","offset":"1e-1","likelihood_shape":"1","log_samples_mean":"0:0","sample_moments":"0:0:0","prior_shape":"1","prior_rate":"0","number_samples":"0"}}},"model":{"weight":{"log_weight":"0","long_term_log_weight":"0"},"prior":{"log_normal":{"decay_rate":"1.666667e-5","offset":"0","gaussian_mean":"0","gaussian_precision":"0","gamma_shape":"1","gamma_rate":"0","number_samples":"0"}}},"model":{"weight":{"log_weight":"0","long_term_log_weight":"0"},"prior":{"normal":{"decay_rate":"1.666667e-5","gaussian_mean":"0","gaussian_precision":"0","gamma_shape":"1","gamma_rate":"0","number_samples":"0"}}},"sample_moments":"0:0:0","decay_rate":"1.666667e-5","number_samples":"0"}},"mode":{"index":"0","prior":{"one-of-n":{"7.1":"","model":{"weight":{"log_weight":"-9.35115518298352","long_term_log_weight":"0"},"prior":{"gamma":{"decay_rate":"1.666667e-5","offset":"1e-1","likelihood_shape":"24.1388909650153","log_samples_mean":"449.571830398649:6.88716203006907","sample_moments":"449.571830398649:1000.22712712371:39163.4813871217","prior_shape":"1","prior_rate":"0","number_samples":"449.5318"}}},"model":{"weight":{"log_weight":"-14.3154312795707","long_term_log_weight":"0"},"prior":{"log_normal":{"decay_rate":"1.666667e-5","offset":"0","gaussian_mean":"6.888135","gaussian_precision":"447.312","gamma_shape":"224.6557","gamma_rate":"9.74089135378146","number_samples":"447.312"}}},"model":{"weight":{"log_weight":"0","long_term_log_weight":"0"},"prior":{"normal":{"decay_rate":"1.666667e-5","gaussian_mean":"1000.141","gaussian_precision":"447.312","gamma_shape":"224.6557","gamma_rate":"8784605.84648132","number_samples":"447.312"}}},"sample_moments":"447.3582:999.6407:39164.04","decay_rate":"1.666667e-5","number_samples":"447.312"}}},"decay_rate":"1.666667e-5","number_samples":"447.312"}}},"sample_moments":"447.3536:999.6405:39163.97","decay_rate":"1.666667e-5","number_samples":"447.3072"}}}


import json
import sys
import sh
import argparse
from math import exp


def jq_reformat(input):
    "Reformat the almost-but-not-quite JSON input to remove duplicate object names to make it acceptable to the json parser"
    json_string = sh.jq('-cn', '--stream', 'def fromstream_with_dups(i):\
  foreach i as $i (\
    [null, null];\
\
    if ($i | length) == 2 then\
      if ($i[0] | length) == 0 then .\
      elif $i[0][-1]|type == "string" then\
        [ ( .[0] | setpath($i[0]; getpath($i[0]) + [$i[1]]) ), .[1] ]\
      else [ ( .[0] | setpath($i[0]; $i[1]) ), .[1] ]\
      end\
    elif ($i[0] | length) == 1 then [ null, .[0] ]\
    else .\
    end;\
\
    if ($i | length) == 1 then\
      if ($i[0] | length) == 1 then .[1]\
      else empty\
      end\
    elif ($i[0] | length) == 0 then $i[1]\
    else empty\
    end\
  );\
  fromstream_with_dups(inputs)', _in=input, _tty_out=False)
    return str(json_string)

def parse_model_state_json(json_string):
    try:
        obj = json.loads(json_string)
        if 'index'  in obj:
            print("Residual data for index id {}".format(obj['index']['_id'][0]))
        elif 'residual_model' in obj:
            model = obj['residual_model']['one-of-n']['model']
            log_weights = model['weight']['log_weight']
            priors = model['prior']
            prior_stats = []
            for name, prior in priors.items():
                if name == 'multimodal':
                    continue
                mean = prior['mean'][0]
                sd = prior['standard_deviation'][0]
                prior_stats.append((name, mean, sd))
            stats = list(zip(log_weights, prior_stats))
            for stat in stats:
                name = stat[1][0]
                weight = stat[0]
                mean = stat[1][1]
                sd = stat[1][2]
                print("\t{name}: weight = {weight:f}, mean = {mean}, sd = {sd}"
                        .format(name=name, weight=exp(float(weight)), mean=mean, sd=sd))
        else:
            pass
    except:
        sys.exit("Error: Cannot parse JSON document. Encountered " + str(sys.exc_info()[0]))

    return

if __name__ == '__main__':

    data=''
    parser = argparse.ArgumentParser(description="Parse a sequence of model state documents in the model_extractor\
            \"JSON\" output format, from file or stdin")
    parser.add_argument("infile", nargs='?', type=argparse.FileType('r'), default=sys.stdin, help="input file")

    args = parser.parse_args()
    data=args.infile.read()

    reformatted_json = jq_reformat(data)

    # Input is expected to be in standard ES Ml format with each document separated by a newline followed by a zero byte
    [parse_model_state_json(line) for line in reformatted_json.splitlines() if line != '0']
