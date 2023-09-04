// scroll up btn
let scrollUpBtn = document.querySelector(".go-up-btn");

window.addEventListener("scroll", (_) => {
	if (window.scrollY >= 700) {
		scrollUpBtn.style.transform = "translate(50%)";
		scrollUpBtn.addEventListener("click", (_) => {
			window.scrollTo({
				top: 0,
				behavior: "smooth",
			});
		});
	} else scrollUpBtn.style.transform = "translate(250%)";
});

// swipe for more code
let swipeForMore = document.querySelector(".swipe-for-more");
swipeForMore.addEventListener("click", aboutScroll);

function aboutScroll() {
	window.scrollTo({
		top: 742,
		behavior: "smooth",
	});
}
// nav bullet scrool code Start
let navBullet = document.querySelector(".nav-bullet");

window.addEventListener("scroll", (_) => {
	if (window.scrollY >= 318) {
		navBullet.style.top = "60px";
	} else {
		navBullet.style.top = "15px";
	}
	if (window.scrollY >= 2300) {
		navBullet.style.top = "105px";
	}
	if (window.scrollY >= 2852) {
		navBullet.style.top = "150px";
	}
	if (window.scrollY >= 4000) {
		navBullet.style.top = "195px";
	}
});

// nav bullet scrool code End

// nav mobile btn code Start

let navBtnMobile = document.querySelector(".nav-btn-mobile"),
	navUl = document.querySelector(".welcome-text-header .nav-links ul"),
	navActiveLayer = document.querySelector(".nav-active-layer");

navBtnMobile.addEventListener("click", (_) => {
	if (navUl.classList.contains("active")) {
		navUl.classList.remove("active");
		navActiveLayer.style.display = "none";
	} else {
		navUl.classList.add("active");
		navActiveLayer.style.display = "block";
	}
});
navActiveLayer.addEventListener("click", (_) => {
	navUl.classList.remove("active");
	navActiveLayer.style.display = "none";
});
// nav mobile btn code End

// start scrolling code

let navLinks = document.querySelectorAll(
	".welcome-text-header .nav-links ul li a"
);

navLinks.forEach((e) => {
	e.addEventListener("click", (x) => {
		x.preventDefault();
		window.scrollTo({
			top: e.getAttribute("data-top"),
			behavior: "smooth",
		});
	});
});

// end scrolling code

// learning map animation

let mapLine = document.querySelector(".map-center-line"),
	mapBoxesSkillsNamesLeft = document.querySelector(
		".about-info .map-section .map-section-content .map-area.left"
	),
	mapBoxesSkillsNamesRight = document.querySelector(
		".about-info .map-section .map-section-content .map-area.right"
	);

// creata function for learning map scroll animation
function learningMapScroll() {
	// start one box in two areas
	if (scrollY >= 1200) {
		mapBoxesSkillsNamesLeft.querySelector(":first-child").style.opacity = "1";
	} else {
		mapBoxesSkillsNamesLeft.querySelector(":first-child").style.opacity = "0";
	}
	if (scrollY >= 1350) {
		mapBoxesSkillsNamesRight.querySelector(":first-child").style.opacity = "1";
	} else {
		mapBoxesSkillsNamesRight.querySelector(":first-child").style.opacity = "0";
	}
	// End one box in two areas
	// start Two box in tow areas
	if (scrollY >= 1550) {
		mapBoxesSkillsNamesLeft.querySelector(":nth-child(2)").style.opacity = "1";
	} else {
		mapBoxesSkillsNamesLeft.querySelector(":nth-child(2)").style.opacity = "0";
	}
	if (scrollY >= 1650) {
		mapBoxesSkillsNamesRight.querySelector(":nth-child(2)").style.opacity = "1";
	} else {
		mapBoxesSkillsNamesRight.querySelector(":nth-child(2)").style.opacity = "0";
	}
	// End Two box in tow areas
	// start last box in left area
	if (scrollY >= 1800) {
		mapBoxesSkillsNamesLeft.querySelector(":last-child").style.opacity = "1";
	} else {
		mapBoxesSkillsNamesLeft.querySelector(":last-child").style.opacity = "0";
	}
	if (scrollY >= 2000) {
		mapBoxesSkillsNamesRight.querySelector(":last-child").style.opacity = "1";
	} else {
		mapBoxesSkillsNamesRight.querySelector(":last-child").style.opacity = "0";
	}
	// End last box in left area
}
window.addEventListener("scroll", learningMapScroll);

// create function animation skills on scroll

function skillsAnimation() {
	let progress = document.querySelectorAll(
		".skills-section .skills-area .skill-box .progress-area .progress"
	);
	window.addEventListener("scroll", (_) => {
		if (window.scrollY >= 2289) {
			progress.forEach((e) => {
				e.style.width = e.dataset.progress + "%";
			});
		}
	});
}

skillsAnimation();

//create function to skills calc animation

let progressNumber = document.querySelectorAll(
	".skills-section .skills-area .skill-box .progress-area .pregress-number"
);
let started = false;

window.addEventListener("scroll", (_) => {
	if (window.scrollY >= 2289) {
		if (!started) {
			progressNumber.forEach((e) => {
				calcSkills(e);
			});
		}
		started = true;
	}
});

function calcSkills(el) {
	let number = el.dataset.progress;
	let count = setInterval(() => {
		el.textContent++;
		if (el.textContent == number) {
			clearInterval(count);
		}
	}, 1000 / number);
}

// icon hover information show

let githubIcon = document.querySelectorAll(
	".project-section .project-area .project-box .hover-box .icon"
);

// create function for create info box and styling

function iconInfoEnter() {
	githubIcon.forEach((e) => {
		e.addEventListener("mouseenter", () => {
			let mainBox = document.createElement("div");
			mainBox.className = "main-box";
			let infoText = document.createElement("div");
			infoText.className = "info-text";
			infoText.innerHTML = e.getAttribute("data-info");
			mainBox.appendChild(infoText);
			e.appendChild(mainBox);
		});
		e.addEventListener("mouseleave", (_) => {
			let ele = e.querySelector(".main-box");
			ele.remove();
		});
	});
}

iconInfoEnter();

// change nav color on footer

let navLinksMain = document.querySelector(".welcome-text-header .nav-links");

window.addEventListener("scroll", (_) => {
	if (scrollY >= 3868) {
		navLinksMain.classList.add("footer-close-nav");
	} else {
		navLinksMain.classList.remove("footer-close-nav");
	}
});

// native copyright year
let copyYear = document.querySelector(".copy-year");

copyYear.innerHTML = new Date().getFullYear();


// * dark mode

let primary = document.querySelector(":root");
let dark_btn = document.querySelector(".dark-btn");


dark_btn.addEventListener("click", ()=>
{
	let element = getComputedStyle(primary);
	if (element.getPropertyValue('--white-color') == '#f1f8fd')
	{
		primary.style.setProperty('--white-color', '#1d1f20');
		primary.style.setProperty('--blue-color', '#c9e4f7');
	}
	else
	{
		primary.style.setProperty('--white-color', '#f1f8fd');
		primary.style.setProperty('--blue-color', '#0f1f24');
	}
});