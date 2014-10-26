package main

import (
	"bytes"
	"encoding/json"
	"encoding/xml"
	"fmt"
	"io"
	"io/ioutil"
	"net/http"
	"net/http/cookiejar"
	"os"
	"strconv"
	"strings"
)

var cookieJar, _ = cookiejar.New(nil)

var client = &http.Client{
	Jar: cookieJar,
}

type CountryData struct {
	XMLName xml.Name  `xml:"bbapi"`
	Country []Country `xml:"countries>country"`
}

type Country struct {
	XMLName   xml.Name `xml:"country"`
	ID        string   `xml:"id,attr"`
	Divisions string   `xml:"divisions,attr"`
	Users     string   `xml:"users,attr"`
	Name      string   `xml:",chardata"`
	NameEn    string
}

func Login(username, password *string) bool {
	fmt.Print("\n\tLogging in... ")

	url := "http://bbapi.buzzerbeater.com/login.aspx?" +
		"login=" + *username + "&code=" + *password

	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	if err != nil {
		logger.Println(err)
		return false
	}

	fmt.Print("Ok!")

	return bytes.Contains(data, []byte("loggedIn"))
}

func Countries() *CountryData {
	fmt.Print("\n\tFetching country data... ")

	url := "http://bbapi.buzzerbeater.com/countries.aspx"

	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return nil
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	countries := new(CountryData)
	err = xml.Unmarshal(data, countries)
	if err != nil {
		logger.Println(err)
		return nil
	}

	fmt.Print("Ok!")

	return countries
}

func TranslatedNames() []string {
	fmt.Print("\n\tFetching translated names... ")

	url := "https://raw.githubusercontent.com/" +
		"rsxee/NTScout/master/names-en.txt"

	res, err := client.Get(url)
	if err != nil {
		return nil
	}
	defer res.Body.Close()

	data, err := ioutil.ReadAll(res.Body)
	if err != nil {
		logger.Println(err)
		return nil
	}

	fmt.Print("Ok!")

	return strings.Split(string(data), "\n")
}

func Releases(ntsver string) string {
	fmt.Print("\n\tLooking for new releases... ")

	url := "https://api.github.com/repos/rsxee/NTScout/releases"
	res, err := client.Get(url)
	if err != nil {
		logger.Println(err)
		return ""
	}
	defer res.Body.Close()
	data, err := ioutil.ReadAll(res.Body)
	if err != nil {
		logger.Println(err)
		return ""
	}

	type Releases struct {
		Url      string
		Tag_name string
		Assets   []struct {
			Browser_download_url string
		}
	}
	var releases []Releases
	err = json.Unmarshal(data, &releases)
	if err != nil {
		logger.Println(err)
		return ""
	}

	if len(releases) == 0 {
		fmt.Print("\n\tNo releases found!")
		return ""
	}

	if !IsNewRelease(ntsver, releases[0].Tag_name) {
		fmt.Print("\n\tNo new releases found!")
		return ""
	}

	fmt.Print("Ok!")

	return releases[0].Assets[0].Browser_download_url
}

func Atoi(s string) int {
	ret, _ := strconv.Atoi(s)
	return ret
}

func IsNewRelease(nts, tag string) bool {
	nts_split := strings.Split(nts, ".")
	tag_split := strings.Split(tag, ".")
	nts_major, nts_minor := Atoi(nts_split[0]), Atoi(nts_split[1])
	tag_major, tag_minor := Atoi(tag_split[0]), Atoi(tag_split[1])

	return tag_major > nts_major ||
		(tag_major == nts_major && tag_minor > nts_minor)
}

func DownloadRelease(url, dir string) bool {
	fmt.Print("\n\tDownloading new release... ")

	res, err := client.Get(url)
	defer res.Body.Close()
	if err != nil {
		logger.Println(err)
		return false
	}

	out, err := os.Create(dir + "/NTScout.zip")
	defer out.Close()
	if err != nil {
		logger.Println(err)
		return false
	}

	_, err = io.Copy(out, res.Body)
	if err != nil {
		logger.Println(err)
		return false
	}

	fmt.Print("Ok!")
	return true
}

func DownloadFlags(id, dir string) bool {
	fmt.Printf("\n\tDownloading flag: %s... ", id)

	url := "http://www.buzzerbeater.com/images/flags/flag_" + id + ".gif"
	res, err := client.Get(url)
	defer res.Body.Close()
	if err != nil {
		logger.Println(err)
		return false
	}

	if res.StatusCode != 404 {
		out, err := os.Create(dir + "/flags/flag_" + id + ".gif")
		defer out.Close()
		if err != nil {
			logger.Println(err)
			return false
		}

		_, err = io.Copy(out, res.Body)
		if err != nil {
			logger.Println(err)
			return false
		}
	}

	fmt.Print("Ok!")
	return true
}
