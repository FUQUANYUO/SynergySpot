/**
 * Copyright (c) 2023 Tencent. All rights reserved.
 * Module: ysd copyrighted music
 * Function: Used to download copyrighted music data
 * This function is not packaged in the SDK by default. If you want to use the functions in this file, please contact Tencent to provide a separate SDK.
 */
#ifndef __ITXCOPYRIGHTEDMEDIA_H__
#define __ITXCOPYRIGHTEDMEDIA_H__

#if defined(_WIN32)
#define COPYRIGHT_API __declspec(dllexport)
#elif __APPLE__
#include <TargetConditionals.h>
#define COPYRIGHT_API __attribute__((visibility("default")))
#elif __ANDROID__
#define COPYRIGHT_API __attribute__((visibility("default")))
#else
#define COPYRIGHT_API
#endif

namespace liteav {

class ITXCopyrightedSongScore;

/**
 * The authentication parameters of YSD
 */
struct TXCopyrightedMediaYSDAuthParams {
    /// User ID registered in YSD
    const char* userId;

    /// User Token
    const char* userToken;

    /// Device ID
    const char* deviceId;

    /// pID registered in YSD
    const char* appId;

    /// pKey provided from YSD
    const char* appKey;
};

/**
 * Copyrighted music enumeration value definition
 */
enum class TXCopyrightedError {

    /// success
    ERR_NONE = 0,

    /// User cancel
    ERR_CANCEL = -1,

    /// token expired
    ERR_TOKEN_OVERDUE = -2,

    /// network error
    ERR_NET_FAILED = -3,

    /// Internal error
    ERR_INNER_ERROR = -4,

    /// License verification fails
    ERR_LICENSE_FAILED = -5,

    /// Music downloading
    ERR_MUSIC_IS_DOWNLOADING = -6,

    /// Accompaniment file does not exist
    ERR_ACCOMPANIMENT_NOT_EXIST = -7,

    /// Original song file does not exist
    ERR_ORIGIN_NOT_EXIST = -8,

    /// Lyrics file does not exist
    ERR_LYRIC_NOT_EXIST = -9,

    /// pitch file does not exist
    ERR_MIDI_NOT_EXIST = -10,

    /// prepare score module failed.
    ERR_PREPARE_FAILED = -11,

    /// call score interface, but not prepare.
    ERR_NOT_PREPARE = -12,

};

class ITXMediaPreloadCallback {
   public:
    /**
     * Copyrighted music starts download callback
     */
    virtual void onPreloadStart(const char* musicId, const char* bitrateDefinition) = 0;

    /**
     * Copyrighted music download progress callback
     */
    virtual void onPreloadProgress(const char* musicId, const char* bitrateDefinition, float progress) = 0;

    /**
     * Copyrighted music download complete callback
     */
    virtual void onPreloadComplete(const char* musicId, const char* bitrateDefinition, int errorCode, const char* msg) = 0;

   protected:
    virtual ~ITXMediaPreloadCallback() = default;
};

class ITXCopyrightedMedia {
   public:
    virtual ~ITXCopyrightedMedia() = default;

    /**
     * set license
     *
     * @param key
     * @param license_url
     */
    virtual void setCopyrightedLicense(const char* key, const char* license_url) = 0;

    /**
     * Set up YSD authentication parameters
     *
     * @param TXCopyrightedMediaYSDAuthParams 音速达鉴权信息
     */
    virtual void setYSDAuthParams(const TXCopyrightedMediaYSDAuthParams& params) = 0;

    /**
     * Preload YSD music resources
     *
     * @param musicId YSD music ID
     * @param bitrateDefinition Bit rate, pass null as the default audio bit rate, the general format is: audio/mi:
     * 32,audio/lo: 64,audio/hi: 128
     */
    virtual void preloadMusic(const char* musicId, const char* bitrateDefinition) = 0;

    /**
     * Generate music URI
     *
     * App Client Called during playback, passed to trtc for playback. correspondence with preloadMusic
     * @param musicId music Id
     * @param bgmType 0：original singer，1：accompaniment  2:  lyrics
     * @param out The buffer passed in by the user is used to store the string returned by genMusicURI
     * @param out_size The size of the user passed buffer
     * @return success：true fail：false
     */
    virtual bool genMusicURI(const char* musicId, int bgmType, const char* bitrateDefinition, char* out, int out_size) = 0;

    /**
     * set music preload callback
     *
     * @param callback preload end object
     */
    virtual void setMusicPreloadCallback(ITXMediaPreloadCallback* callback) = 0;

    /**
     * Preload music data.
     *
     * @param musicId music Id
     * @param playToken play Token
     * @param bitrateDefinition Bit rate, pass nil as the default audio bit rate, the general format is: audio/mi:
     * 32,audio/lo: 64,audio/hi: 128
     * @param callback callback object
     */
    virtual void preloadMusic(const char* musicId, const char* bitrateDefinition, const char* playToken) = 0;

    /**
     * cancel preload music data.
     *
     * @param musicId music id
     * @param bitrateDefinition Bit rate, pass nil as the default audio bit rate
     */
    virtual void cancelPreloadMusic(const char* musicId, const char* bitrateDefinition) = 0;

    /**
     * Detect if music data has been preloaded
     *
     * @param musicId music id
     * @param bitrateDefinition Bit rate, pass nil as the default audio bit rate
     */
    virtual bool isMusicPreload(const char* musicId, const char* bitrateDefinition) = 0;

    /**
     * clear cache
     */
    virtual void clearMusicCache() = 0;

    /**
     * Set the maximum number of song caches, the default is 100
     *
     * @param maxCount maximum number of songs
     */
    virtual void setMusicCacheMaxCount(int maxCount) = 0;

    /**
     * Create song score instance
     *
     * @param sampleRate    sample rate
     * @param channel       channel
     * @param playToken     play token
     * @param noteFilePath  not file path
     * @param lyricFilePath lyric file path
     */
    virtual ITXCopyrightedSongScore* createSongScore(const char* musicId, int sampleRate, int channel, const char* playToken, const char* noteFilePath, const char* lyricFilePath) = 0;

    /**
     * Destroy song score instance
     */
    virtual void destroySongScore(ITXCopyrightedSongScore** songScore) = 0;
};

/**
 * Pitch data struct
 */
struct ITXCopyrightedSongScoreNoteItem {
    /// start time
    int startTime;

    /// duration
    int duration;

    /// pitch
    int pitch;
};

class ITXCopyrightedSongScoreCallback {
   public:
    /**
     * Output the score of each lyric
     *
     * @param currentScore  score for this sentence
     * @param totalScore    total score
     * @param curIndex      lyric index
     */
    virtual void onMIDISCoreUpdate(const char* musicId, int currentScore, int totalScore, int curIndex) = 0;

    /**
     * Outputs real-time pitch hit.
     *
     * @param isHit         is hit
     * @param timeStamp     current time
     * @param pitch         The user's pitch in the note time range
     *   (The value can be compared with the note pitch, -1 means it is not in the note time range,
     *   greater than -1 means the user pitch)
     * @param viewValue     user's real-time pitch
     */
    virtual void onMIDIGroveAndHint(const char* musicId, bool isHit, float timeStamp, float pitch, int viewValue) = 0;

    /**
     * Score result callback
     *
     * @param scoreArray    A collection of scores for each lyric
     * @param totalScore    total score
     */
    virtual void onMIDIScoreFinish(const char* musicId, int* scoreArray, int arrayCount, int totalScore) = 0;

    /**
     * prepared callback
     *
     * Indicates the scoring module is ready and can call process interface
     */
    virtual void onMIDIScorePrepared(const char* musicId) = 0;

    /**
     * Error callback
     *
     * @param errCode   {@link TXCopyrightedError}
     * @param errMsg    error message
     */
    virtual void onMIDIScoreError(const char* musicId, TXCopyrightedError errCode, const char* errMsg) = 0;

   protected:
    virtual ~ITXCopyrightedSongScoreCallback() = default;
};

class ITXCopyrightedSongScore {
   public:
    virtual ~ITXCopyrightedSongScore() = default;

    /**
     * Set callback
     *
     * @param callback song score callback
     */
    virtual void setSongScoreCallback(ITXCopyrightedSongScoreCallback* callback) = 0;

    /**
     * Init Song score instance
     *
     * success {@link onMIDIScorePrepared} callback
     * fail {@link onMIDIScoreError} callback
     */
    virtual void prepare() = 0;

    /**
     * Stop song score
     */
    virtual void finish() = 0;

    /**
     * Process captured voice pcm data
     *
     * @param pcmData       pcm data
     * @param length        pcm data size
     * @param timeStamp     time stamp
     */
    virtual void process(char* pcmData, int length, float timeStamp) = 0;

    /**
     * set key
     *
     * @param shiftKey key
     */
    virtual void setKeyShift(int shiftKey) = 0;

    /**
     * Calculate total score
     *
     * @return total score
     */
    virtual int calculateTotalScore() = 0;

    /**
     * Get Grove data count
     *
     * @return Grove data count
     */
    virtual int getGroveCount() = 0;

    /**
     * Grove data
     *
     * @param note_item_array Grove data buffer
     * @param array_size Grove data size
     * @return ScoreNoteItem count
     */
    virtual int getAllGrove(ITXCopyrightedSongScoreNoteItem* note_item_array, int array_size) = 0;
};

}  // namespace liteav

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create CopyrightedMedia instance
 */
COPYRIGHT_API liteav::ITXCopyrightedMedia* CreateCopyRightMedia();

/**
 * Destroy CopyrightedMedia instance
 */
COPYRIGHT_API void DestroyCopyRightMedia(liteav::ITXCopyrightedMedia** media);

#ifdef __cplusplus
}
#endif

#endif
