"""Reconstruct magnetic resonance parametres.

This is mainly a re-implementation of scripts provided by Alexandre Vignaud.
"""

from __future__ import division

import itertools
import math
import numpy as np

from soma import aims, aimsalgo

twopi = 2 * np.pi


class BAFIData:
    def __init__(self, amplitude_volume, phase_volume):
        """Pass the BAFI data as two amplitude-phase 4D AIMS volumes.

        The last dimension of both arrays represents the different echos.
        """
        self.amplitude_volume = amplitude_volume
        self.phase_volume = phase_volume
        self.reference_amplitude = None
        # TODO where do I find these values?
        self.threshold = 40
        self.prescribed_flip_angle = 60.0  # degrees
        self.echo_times = [3.061, 3.061, 4.5, 7.0]  # milliseconds
        self.TR_factor = 5.0
        self.tau = 1.2e-3  # RF pulse duration in seconds TODO check real value!!

    def make_B1_map(self, B0_correction=False):
        """Build a map of B1 (in radians) from BAFI data.

        Return a numpy array of complex type.

        This is a re-implementation of BAFI2B1map.m, courtesy of Alexandre Vignaud.

        %% The method is Yarnykh's (MRM 57:192-200 (2007)) +
        %% Amadon ISMRM2008 (MAFI sequence: simultaneaous cartography of B0 and B1)
        """
        BAFI_amplitude = np.asarray(self.amplitude_volume)
        BAFI_phase = np.asarray(self.phase_volume)
        signal_echo1 = (BAFI_amplitude[:, :, :, 0] *
                        np.exp(1j * BAFI_phase[:, :, :, 0]))
        signal_echo2 = (BAFI_amplitude[:, :, :, 1] *
                        np.exp(1j * BAFI_phase[:, :, :, 1]))
        #mask = signal_echo1 < self.threshold
        #signal_echo1[mask] = 0
        #signal_echo2[mask] = 0
        r = signal_echo2 / signal_echo1
        invalid_mask = np.abs(r) >= 1
        r[invalid_mask] = 0
        # The next line is equivalent to
        #G = np.abs(np.angle(r)) > np.pi / 2;signed_r = (1 - 2 * G) * np.abs(r)
        signed_r = np.copysign(np.abs(r), r.real)
        # r[np.isnan(r)] = 1  # necessary?
        alphaAFI = np.arccos((signed_r * self.TR_factor - 1) / (self.TR_factor - signed_r))
        if B0_correction:
            B0_map = self.make_B0_map()
            B1Mat = self._correctB0(alphaAFI, np.angle(r), B0_map)
        else:
            B1Mat = alphaAFI * (r / np.abs(r))
        B1Mat[invalid_mask] = 0  # necessary?
        return B1Mat / math.radians(self.prescribed_flip_angle)

    def _make_flip_angle_map_original(self):
        """Build a map of actual flip angle (in radians) from BAFI data.

        Return a numpy array of complex type.

        This is a re-implementation of BAFI2FAmap.m, courtesy of Alexandre Vignaud.

        %% The method is Yarnykh's (MRM 57:192-200 (2007)) +
        %% Amadon ISMRM2008 (MAFI sequence: simultaneaous cartography of B0 and B1)
        """
        BAFI_amplitude = np.asarray(self.amplitude_volume)
        BAFI_phase = np.asarray(self.phase_volume)
        signal_echo1 = (BAFI_amplitude[:, :, :, 0] *
                        np.exp(1j * BAFI_phase[:, :, :, 0]))
        signal_echo2 = (BAFI_amplitude[:, :, :, 1] *
                        np.exp(1j * BAFI_phase[:, :, :, 1]))
        #mask = signal_echo1 < self.threshold
        #signal_echo1[mask] = 0
        #signal_echo2[mask] = 0
        r = signal_echo2 / signal_echo1
        invalid_mask = np.abs(r) >= 1
        #r[invalid_mask] = 0
        # r[np.isnan(r)] = 1  # necessary?
        alphaAFI = np.arccos((np.abs(r) * self.TR_factor - 1) / (self.TR_factor - np.abs(r)))
        B1Mat = alphaAFI * (r / np.abs(r))
        #B1Mat[invalid_mask] = 0  # necessary?
        return B1Mat

    def make_flip_angle_map(self):
        """Build a map of actual flip angle (in radians) from BAFI data.

        This is a re-implementation of BAFI2FAmap.m (courtesy of Alexandre Vignaud)
        modified to return only the real flip angle (omitting the phase).

        %% The method is Yarnykh's (MRM 57:192-200 (2007)) +
        %% Amadon ISMRM2008 (MAFI sequence: simultaneaous cartography of B0 and B1)
        """
        BAFI_amplitude = np.asarray(self.amplitude_volume)
        signal_echo1 = BAFI_amplitude[:, :, :, 0]
        signal_echo2 = BAFI_amplitude[:, :, :, 1]
        abs_r = signal_echo2 / signal_echo1
        fa = np.arccos((abs_r * self.TR_factor - 1) / (self.TR_factor - abs_r))
        fa[abs_r >= 1] = 0
        return fa

    def make_B0_map(self):
        """Build a map of B0 in Hz from BAFI data.

        Return the map as a numpy array.

        This is a re-implementation of Phase2B0Map.m, courtesy of Alexandre Vignaud.
        """
        BAFI_amplitude = np.asarray(self.amplitude_volume)
        BAFI_phase = np.asarray(self.phase_volume)
        nechos = BAFI_amplitude.shape[3]
        DTE1 = self.echo_times[2] - self.echo_times[1]
        DTE2 = self.echo_times[3] - self.echo_times[2]
        tau = np.array([0.0, DTE1, DTE1 + DTE2])
        sumtau = tau.sum()
        sumtau2 = (tau ** 2).sum()
        assert nechos == 4
        D = [BAFI_phase[:, :, :, 0] / twopi,
             BAFI_phase[:, :, :, 2] / twopi,
             BAFI_phase[:, :, :, 3] / twopi]  # specific to 4 echos
        D1 = D[1] - D[0]
        D1neg = (D1 < -0.5)
        D1pos = (D1 >= 0.5)
        D1 += D1neg - D1pos
        D[1] += D1neg - D1pos
        # estimation of 3rd echo phase excursion from first 2 echoes
        D2th = D1 * (DTE1 + DTE2) / DTE1
        D[2] += np.rint(D2th)
        D2 = D[2] - D[0]
        eps = D2 - D2th
        D2neg = (eps < -0.5)
        D2pos = (eps >= 0.5)
        D2 += D2neg - D2pos
        D[2] += D2neg - D2pos
        Sxy = tau[1] * D[1] + tau[2] * D[2]
        Sy = D[0] + D[1] + D[2]
        return (3 * Sxy - sumtau * Sy) / (3 * sumtau2 - sumtau ** 2)

    def _correctB0(self, FA_map, FA_phase, B0_map):
        """Apply B0 correction to a B1 map.

        This is a re-implementation of correctB0.m, courtesy of Alexandre Vignaud.
        """
        return self.correctB0(FA_map, FA_phase, B0_map,
                              self.tau, self.echo_times[0])

    @staticmethod
    def correctB0(FA_map, FA_phase, B0_map, tau, echo_time):
        """Apply B0 correction to a B1 map.

        This is a re-implementation of correctB0.m, courtesy of Alexandre Vignaud.
        """
        d = twopi * tau * B0_map
        a = d ** 2 / 240
        b = np.sqrt(((2 - 2 * np.cos(d)) / (d ** 2)))
        # b[np.isnan(b)] = 0  # necessary?
        FAcor = FA_map / b + a * (FA_map ** 3) / (b ** 4 - 3 * a * b * (FA_map ** 2))
        # make FAcor finite by replacing with FA values where not finite, seems necessary
        notfinite = ~np.isfinite(FAcor)
        FAcor[notfinite] = FA_map[notfinite]
        if FA_phase is not None:
            D = np.sqrt(FAcor ** 2 + d ** 2) / 2
            FA_phase -= (twopi * echo_time * B0_map -
                         np.arctan2(-2 * D * np.cos(D), d * np.sin(D)))
            return FAcor * np.exp(1j * FA_phase)
        else:
            return FAcor

    def afi1_noT2((T1, M0, FA_factor), BAFI_Data):
        n = BAFI_Data.TR_factor
        TR1 = BAFI_Data.repetition_time / (n + 1)
        TR2 = n * TR1
        E1 = math.exp(-TR1 / T1)
        E2 = math.exp(-TR2 / T1)
        flip_angle = BAFI_Data.prescribed_flip_angle * FA_factor
        return (M0 * math.sin(flip_angle) *
                (1 - E2 + (1 - E1) * E2 * math.cos(flip_angle)) /
                (1 - E1 * E2 * np.cos(flip_angle) ** 2))

    def afi2_noT2((T1, M0, FA_factor), BAFI_Data):
        n = BAFI_Data.TR_factor
        TR1 = BAFI_Data.repetition_time / (n + 1)
        TR2 = n * TR1
        E1 = math.exp(-TR1 / T1)
        E2 = math.exp(-TR2 / T1)
        flip_angle = BAFI_Data.prescribed_flip_angle * FA_factor
        return (M0 * math.sin(flip_angle) *
                (1 - E1 + (1 - E2) * E1 * math.cos(flip_angle)) /
                (1 - E1 * E2 * np.cos(flip_angle) ** 2))

    def fix_b1_map(self, b1map, smooth_type='median', gaussian=False,
                   output_median=False):
        '''Fix/improve the B1 map by filling holes, smoothing, and extending it
        a little bit spacially so as to use it on the complete whole brain.

        Parameters
        ----------
        b1map: volume
            the B1 map to be corrected, may be the output of
            self.make_flip_angle_map()
        smooth_type: str (optional)
            smoothing correction type. default: 'median'
            median:
            dilated:
        gaussian: float (optional)
            default: 0 (not applied)
            perform an additional gaussian filtering of given stdev
        output_median: bool (optional)
            if set, the output will be a tuple including a 2nd volume:
            the median-filtered B1 map. Only valid if smooth_type is 'median'.

        Returns
        -------
        The corrected B1 map.
        If output_median is set, the return value is a tuple
        (corrected B1 map, median-filtered B1 map)
        '''
        B1map_volume = b1map
        voxel_size = B1map_volume.getVoxelSize()[:3]
        data_type = aims.typeCode(str(np.asarray(B1map_volume).dtype))
        if smooth_type == 'dilated':
            morpho = getattr(aimsalgo, 'MorphoGreyLevel_' + data_type)()
            B1map_volume = morpho.doDilation(
                B1map_volume, max(voxel_size)*2)
        elif smooth_type == 'median':
            # median filtering is meant to fill small holes in the B1 map
            # use a larger volume (border) to get it done in the border layer
            volume_type = B1map_volume.__class__
            vol_border = volume_type(B1map_volume.getSizeX(),
                B1map_volume.getSizeY(),
                B1map_volume.getSizeZ(),
                B1map_volume.getSizeT(), 1)
            np.asarray(vol_border)[:,:,:,0] = np.asarray(B1map_volume)[:,:,:,0]
            vol_border.copyHeaderFrom(B1map_volume.header())
            vol_border.refVolume().copyHeaderFrom(B1map_volume.header())
            median = getattr( aimsalgo, 'MedianSmoothing_' + data_type)
            # apply the filter on the larger image since the filter actually only
            # applies to the interior limited by the mask size
            B1map_volume_med_border \
                = median().doit(vol_border.refVolume()).volume()
            # get a smaller view in the result
            B1map_volume_med = volume_type(B1map_volume_med_border,
                volume_type.Position4Di(1,1,1,0),
                volume_type.Position4Di(*B1map_volume.getSize()))
            B1map_volume_med_arr = np.asarray(B1map_volume_med)
            B1map_volume_med_arr[np.isnan(B1map_volume_med_arr)] = 0
            # dilate the map to extrapolate outside the brain
            morpho = getattr(aimsalgo, 'MorphoGreyLevel_' + data_type)()
            B1map_volume_old = B1map_volume
            B1map_volume = morpho.doDilation(
                B1map_volume_med, max(voxel_size)*4)
            # "un-filter" the part which already had valid data
            B1map_ar = np.asarray(B1map_volume_old)
            np.asarray(B1map_volume)[B1map_ar>1e-2] = B1map_ar[B1map_ar>1e-2]
        if gaussian != 0:
            gsmooth = getattr( aimsalgo, 'Gaussian3DSmoothing_' + data_type)
            B1map_volume = gsmooth(gaussian, gaussian,
                gaussian).doit(B1map_volume).volume()

        if output_median and smooth_type == 'median':
            return B1map_volume, B1map_volume_med
        else:
            return B1map_volume


class GREData2FlipAngles:
    def __init__(self, min_FA_volume, max_FA_volume):
        self.volumes = [min_FA_volume, max_FA_volume]
        self.reference_amplitude = None
        self.flip_angles = [5, 20]  # must be in ascending order
        self.repetition_time = 14


def correct_sensitivity_map(sensitivity_map, flip_angle_map):
    """Factor out the effect of the transmit field from a sensitivity map."""
    # TODO threshold [0,2] on flip angle map (necessary?)
    return sensitivity_map / np.abs(flip_angle_map)


def t1mapping_VFA(flip_angle_factor, GRE_data):
    """Reconstruct a T1 relaxometry map from DESPOT1 data (2 flip angles).

    This is a re-implementation of GetT1_VAFI3Dp_Brebis.m, courtesy of
    Alexandre Vignaud.
    """
    flip_angle_factor = np.asarray(flip_angle_factor)
    S1 = np.asarray(GRE_data.volumes[0])
    S2 = np.asarray(GRE_data.volumes[1])
    A1 = flip_angle_factor * math.radians(GRE_data.flip_angles[0])
    A2 = flip_angle_factor * math.radians(GRE_data.flip_angles[1])
    sA1 = np.sin(A1)
    sA2 = np.sin(A2)
    cA1 = np.cos(A1)
    cA2 = np.cos(A2)
    p = (S2 * sA1 - S1 * sA2) / (S2 * sA1 * cA2 - S1 * sA2 * cA1)
    print 'negative p:', p[p<=0]
    epsilon = 1e-3
    p[p<=0] = epsilon
    T1 = np.real(-GRE_data.repetition_time / np.log(p))
    # zero out non-finite T1 values (necessary?)
    # zero out non-real T1 values (necessary?)
    # mask out result (necessary?)
    #T1[T1<0] = 0
    # threshold out values outside [0, 10000ms]
    #sup_bound = 10000
    #T1[T1>sup_bound] = sup_bound
    #T1[np.isnan(T1)] = 0
    return T1
