/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "titanic/sound/sound_manager.h"
#include "titanic/titanic.h"
namespace Titanic {

const uint SAMPLING_RATE = 22050;
const uint LATENCY = 100;
const uint CHANNELS_COUNT = 16;

CSoundManager::CSoundManager() : _musicPercent(75.0), _speechPercent(75.0),
	_masterPercent(75.0), _parrotPercent(75.0), _field14(1) {
}

/*------------------------------------------------------------------------*/

void QSoundManagerSounds::add(CWaveFile *waveFile, int iChannel, CEndTalkerFn endFn, TTtalker *talker) {
	push_back(new QSoundManagerSound(waveFile, iChannel, endFn, talker));
}

void QSoundManagerSounds::flushChannel(int iChannel) {
	for (iterator i = begin(); i != end(); ++i) {
		QSoundManagerSound *item = *i;
		if (item->_iChannel == iChannel) {
			if (item->_endFn)
				item->_endFn(item->_talker);

			remove(item);
			delete item;
			break;
		}
	}
}

void QSoundManagerSounds::flushChannel(int v1, int iChannel) {
	for (iterator i = begin(); i != end(); ++i) {
		QSoundManagerSound *item = *i;
		if (item->_waveFile->isLoaded() && item->_iChannel == iChannel) {
			if (item->_endFn)
				item->_endFn(item->_talker);

			remove(item);
			delete item;
			break;
		}
	}
}

bool QSoundManagerSounds::contains(const CWaveFile *waveFile) const {
	for (const_iterator i = begin(); i != end(); ++i) {
		const QSoundManagerSound *item = *i;
		if (item->_waveFile == waveFile)
			return true;
	}

	return false;
}


/*------------------------------------------------------------------------*/

QSoundManager::QSoundManager(Audio::Mixer *mixer) : CSoundManager(), QMixer(mixer),
		_field18(0), _field1C(0) {
	_slots.resize(48);
	Common::fill(&_channelsVolume[0], &_channelsVolume[16], 0);
	Common::fill(&_channelsMode[0], &_channelsMode[16], 0);

	qsWaveMixInitEx(QMIXCONFIG(SAMPLING_RATE, CHANNELS_COUNT, LATENCY));
	qsWaveMixActivate(true);
	qsWaveMixOpenChannel(0, QMIX_OPENALL);
}

QSoundManager::~QSoundManager() {
	// Close down the mixer
	qsWaveMixCloseSession();
}

CWaveFile *QSoundManager::loadSound(const CString &name) {
	CWaveFile *waveFile = new CWaveFile();

	// Try to load the specified sound
	if (!waveFile->loadSound(name)) {
		delete waveFile;
		return nullptr;
	}

	return waveFile;
}

CWaveFile *QSoundManager::loadSpeech(CDialogueFile *dialogueFile, int speechId) {
	warning("TODO");
	return nullptr;
}

int QSoundManager::proc5() const {
	warning("TODO");
	return 0;
}

int QSoundManager::playSound(CWaveFile &soundRes, CProximity &prox) {
	warning("TODO");
	return 0;
}

void QSoundManager::stopSound(uint handle) {
	warning("TODO");
}

void QSoundManager::proc8(int v) {
	warning("TODO");
}

void QSoundManager::proc9(uint handle) {
	for (uint idx = 0; idx < _slots.size(); ++idx) {
		if (_slots[idx]._handle == handle)
			_slots[idx]._val2 = 1;
	}
}

void QSoundManager::stopAllChannels() {
	qsWaveMixFlushChannel(0, QMIX_OPENALL);

	for (int idx = 0; idx < 16; ++idx)
		_sounds.flushChannel(idx);
	flushChannels(10);
}

void QSoundManager::setVolume(uint handle, uint volume, uint seconds) {
	for (uint idx = 0; idx < _slots.size(); ++idx) {
		Slot &slot = _slots[idx];
		if (slot._handle == handle) {
			_channelsVolume[slot._channel] = volume;
			updateVolume(slot._channel, seconds * 1000);

			if (volume) {
				uint ticks = g_vm->_events->getTicksCount() + seconds * 1000;
				if (!slot._ticks || ticks >= slot._ticks)
					slot._ticks = ticks;
			} else {
				slot._ticks = 0;
			}
			break;
		}
	}
}

void QSoundManager::setVectorPosition(uint handle, double x, double y, double z, uint panRate) {
	for (uint idx = 0; idx < _slots.size(); ++idx) {
		Slot &slot = _slots[idx];
		if (slot._handle == handle) {
			qsWaveMixSetPanRate(slot._channel, QMIX_USEONCE, panRate);
			qsWaveMixSetSourcePosition(slot._channel, QMIX_USEONCE, QSVECTOR(x, y, z));
			break;
		}
	}
}

void QSoundManager::setPolarPosition(uint handle, double range, double azimuth, double elevation, uint panRate) {
	for (uint idx = 0; idx < _slots.size(); ++idx) {
		Slot &slot = _slots[idx];
		if (slot._handle == handle) {
			qsWaveMixSetPanRate(slot._channel, QMIX_USEONCE, panRate);
			qsWaveMixSetPolarPosition(slot._channel, QMIX_USEONCE, 
				QSPOLAR(azimuth, range, elevation));
			break;
		}
	}
}

bool QSoundManager::isActive(uint handle) const {
	for (uint idx = 0; idx < _slots.size(); ++idx) {
		if (_slots[idx]._handle == handle)
			return true;
	}

	return false;
}

bool QSoundManager::isActive(const CWaveFile *waveFile) const {
	return _sounds.contains(waveFile);
}

uint QSoundManager::getLatency() const {
	return LATENCY;
}

void QSoundManager::setMusicPercent(double percent) {
	_musicPercent = percent;
	updateVolumes();
}

void QSoundManager::setSpeechPercent(double percent) {
	_speechPercent = percent;
	updateVolumes();
}

void QSoundManager::setMasterPercent(double percent) {
	_masterPercent = percent;
	updateVolumes();
}

void QSoundManager::setParrotPercent(double percent) {
	_parrotPercent = percent;
}

void QSoundManager::proc29() {
	warning("TODO");
}

void QSoundManager::proc30() {
	warning("TODO");
}

void QSoundManager::soundFreed(Audio::SoundHandle &handle) {
	qsWaveMixFreeWave(handle);
}

void QSoundManager::flushChannels(int channel) {
	int endChannel;
	switch (channel) {
	case 0:
	case 3:
		endChannel = channel + 3;
		break;
	case 6:
		endChannel = 10;
		break;
	case 10:
		endChannel = 48;
		break;
	default:
		return;
	}

	for (; channel < endChannel; ++channel) {
		qsWaveMixFlushChannel(channel);
		_sounds.flushChannel(channel);
	}
}

void QSoundManager::updateVolume(int channel, uint panRate) {
	uint volume = _channelsVolume[channel] * 327;

	switch (_channelsMode[channel]) {
	case 0:
	case 1:
	case 2:
		volume = (_speechPercent * volume) / 100;
		break;
	case 3:
	case 4:
	case 5:
		volume = (24525 * volume) / 100;
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		volume = (_masterPercent * volume) / 100;
		break;
	default:
		break;
	}
	
	volume = (_musicPercent * volume) / 100;
	qsWaveMixSetPanRate(channel, 0, panRate);
	qsWaveMixSetVolume(channel, 0, volume);
}

void QSoundManager::updateVolumes() {
	for (int idx = 0; idx < CHANNELS_COUNT; ++idx)
		updateVolume(idx, 250);
}

} // End of namespace Titanic z