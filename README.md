<h1 align="center">🕹️ Direct3D 11를 활용한 젤다의 전설 꿈꾸는 섬 모작 포트폴리오</h1>
<p align="center">
   DirectX 11의 다양한 기능을 습득하고, 셰이더 프로그래밍 및 3D 애니메이션에 대한 이해도와 구현력을 향상시키는 것을 목표로 했습니다. <br>
   "젤다의 전설 꿈꾸는 섬"의 다양한 콘텐츠를 구현하고, 다양한 기법을 적용해보기에 적합하다고 판단해 선정했습니다. 
</p>

<br>

## 🌟 하이라이트

<table>
  <tr>
    <th style="text-align:center">게임 플레이</th>
    <th style="text-align:center">파티클 이펙트</th>
  </tr>
  <tr>
    <td style="text-align:center">
      <img src="https://github.com/user-attachments/assets/729c09ea-6669-4c9a-85c3-dc59c587d55d" width="400" height="300">
    </td>
    <td style="text-align:center">
      <img src="https://github.com/user-attachments/assets/299272b2-de3d-47c7-8197-6e7247335881" width="400" height="300">
    </td>
  </tr>
  <tr>
    <th style="text-align:center">콜라이더</th>
    <th style="text-align:center">네비게이션 메시</th>
  </tr>
  <tr>
    <td style="text-align:center">
      <img src="https://github.com/user-attachments/assets/3b7ffec0-add0-47ea-a795-6663243ca1fe" width="400" height="300">
    </td>
    <td style="text-align:center">
      <img src="https://github.com/user-attachments/assets/ad20073e-a835-4278-a403-e69c4bcacd00" width="400" height="300">
    </td>
  </tr>
</table>

<br>

## 🔗 링크
- [포트폴리오 영상](https://www.youtube.com/watch?v=mM-L5vRDPjU)

<br>

## 📜 프로젝트 개요
- **기간**: `2023.12.04 ~ 2024.03.11`
- **인원**: `1명`
- **역할**: `엔진`, `클라이언트`

<br>

## ⚙️ 개발 환경
- **개발 언어**: `C++`, `HLSL`
- **개발 툴**: `Visual Studio 2022` 
- **버전 관리 및 협업 도구**: `Git`, `GitHub`
- **렌더링 API**: `Direct3D 11`

<br>

## ⚒️ 구현 사항
- **조명**: `HLSL 셰이더에 바인딩하여 픽셀의 조명 효과를 구현했습니다.`
- **애니메이션**: `애니메이션을 원활하게 재생하기 위해 선형보간을 구현했고, 애니메이션 데이터를 효율적으로 관리할 수 있도록 구현했습니다.`
- **파티클 시스템**: `파티클 시스템을 활용하여 파티클을 모았다가 방출하는 스킬 이펙트를 구현했습니다.`
- **콜라이더**: `콜라이더를 부위별로 세분화하고 컴포넌트화하여 특정 뼈에 부착해 애니메이션 시에도 뼈를 따라가도록 구현했고, 이를 통해 부위별로 데미지를 줄 수도 있고, 받을 수도 있도록 기능을 구현했습니다.`
- **네비게이션 메시**: `지형 셀의 이웃 셀 여부를 판단하는 기능을 구현했습니다. 이를 통해 시계방향으로 그려진 지형 셀이라도 이웃 셀로 등록되어야만 이동할 수 있도록 설정했습니다.`

