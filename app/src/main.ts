import * as dashjs from "dashjs";
const video = document.querySelector("#videoPlayer") as HTMLVideoElement;
const qualityVideoContainer = document.querySelector("#qualityVideoContainer");
const qualityAudioContainer = document.querySelector("#qualityAudioContainer");
const qualityVideoShowContainer = document.querySelector(
  "#qualityVideoShowContainer"
);
const qualityAudioShowContainer = document.querySelector(
  "#qualityAudioShowContainer"
);

const url = "/videos/quality/output.mpd";
const player = dashjs.MediaPlayer().create();
player.initialize(video, url);

player.on("qualityChangeRequested", (e) => {
  console.log(e);
  const newQuality = e.newQuality;
  const mediaType = e.mediaType;
  if (mediaType === "video") {
    qualityVideoShowContainer!.innerHTML = `${
      player.getBitrateInfoListFor("video").at(newQuality)!.height
    }p`;
  }
  if (mediaType === "audio") {
    qualityAudioShowContainer!.innerHTML = `${
      player.getBitrateInfoListFor("audio").at(newQuality)!.bitrate
    }bps`;
  }
});

video.addEventListener("loadedmetadata", () => {
  console.log("loadedmetadata");
  console.log(player.getBitrateInfoListFor("video"));
  console.log(player.getBitrateInfoListFor("audio"));
  console.log(player.duration());
  const videoQualityList = player.getBitrateInfoListFor("video");
  const audioQualityList = player.getBitrateInfoListFor("audio");
  videoQualityList.forEach((quality, index) => {
    const button = document.createElement("button");
    button.innerText = `${quality.height}p`;
    button.addEventListener("click", (e) => {
      player.updateSettings({
        streaming: {
          abr: {
            autoSwitchBitrate: {
              video: false,
            },
          },
        },
      });
      button.style.backgroundColor = "red";
      const buttons = qualityVideoContainer?.querySelectorAll("button");
      buttons?.forEach((button) => {
        if (button !== e.target) {
          button.style.backgroundColor = "white";
        }
      });
      qualityVideoShowContainer!.innerHTML = `${quality.height}p`;
      player.setQualityFor("video", index);
    });
    qualityVideoContainer?.appendChild(button);
  });
  audioQualityList.forEach((quality, index) => {
    const button = document.createElement("button");
    button.innerText = `${quality.bitrate}bps`;
    button.addEventListener("click", (e) => {
      player.updateSettings({
        streaming: {
          abr: {
            autoSwitchBitrate: {
              audio: false,
            },
          },
        },
      });
      button.style.backgroundColor = "red";
      const buttons = qualityAudioContainer?.querySelectorAll("button");
      buttons?.forEach((button) => {
        if (button !== e.target) {
          button.style.backgroundColor = "white";
        }
      });
      qualityAudioShowContainer!.innerHTML = `${quality.bitrate}bps`;
      player.setQualityFor("audio", index);
    });
    qualityAudioContainer?.appendChild(button);
  });
  const videoButtons = qualityVideoContainer?.querySelectorAll("button");
  videoButtons?.forEach((button) => {
    button.style.backgroundColor = "white";
  });
  const audioButtons = qualityAudioContainer?.querySelectorAll("button");
  audioButtons?.forEach((button) => {
    button.style.backgroundColor = "white";
  });
});
